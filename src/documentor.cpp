/*
 * This file is part of Escain Documentor tool
 * 
 * Escain Documentor is free software: you can redistribute it and/or modify 
 * it under ther terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * Escain Documentor is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with Escain Documentor. If not, see <https://www.gnu.org/licenses/>.
 * 
 * Author: Adrian Maire escain (at) gmail.com
 */

#include "documentor.hpp"

#include <string>
#include <string_view>

#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QString>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QWebEngineView>

#include "ui_center.h" // destructor needs it

#define OUTPUT_TO_FILE false

namespace Escain
{

Documentor::Documentor(QWidget *parent)
	: QMainWindow(parent)
	, m_center(this)
	, m_query(QXmlQuery::XSLT20)
{
	setObjectName("EscainDocumentor");
	setWindowTitle("EscainDocumentor");

	setCentralWidget(&m_center);

	setupMenu();
	setupGui();
	
	m_uiCenterPtr->horizontalSplitter->setStretchFactor(0, 0);
	m_uiCenterPtr->horizontalSplitter->setStretchFactor(1, 100);
	
	connect(&m_fileManager, &FileManager::clearContent, [this]()
	{
		m_uiCenterPtr->textEditor->setText(
R"delimiter(<document>
	<title>Document</title>
	<header-1 title="Title">
		<p>Some text <b> in bold </b></p>
	</header-1>
</document>)delimiter");
		onUpdateTimeout();
	});
	
	connect(&m_fileManager, &FileManager::loadContentFrom, [this](std::ifstream& fileStream)
	{
		std::string str;
		fileStream.seekg(0, std::ios::end);   
		str.reserve(fileStream.tellg());
		fileStream.seekg(0, std::ios::beg);
		
		str.assign((std::istreambuf_iterator<char>(fileStream)),
		    std::istreambuf_iterator<char>());
		
		m_uiCenterPtr->textEditor->setText(QString::fromStdString(str));
		onUpdateTimeout();
		return true;
	});
	
	connect(&m_fileManager, &FileManager::extractContentTo, [this](std::ofstream& fileStream)
	{
		fileStream << m_uiCenterPtr->textEditor->toPlainText().toStdString();
		
		return true;
	});
	
	connect(&m_fileManager,&FileManager::setFileTitle, 
	    [this](const std::string& s){setWindowTitle(QString::fromStdString(s));});
	
	m_timer.setInterval(s_msDelayToUpdate);
	m_timer.setSingleShot(true);
	connect(&m_timer, &QTimer::timeout, [this](){ onUpdateTimeout(); });
	
	// connect update content
	connect(m_uiCenterPtr->textEditor->document(), 
	    &QTextDocument::contentsChanged, [this](){ contentChanged(); });
	
	onUpdateTimeout();
	
	// Pre-load xslt
	QFile file(":/documentation.xsl");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Could not open resource xslt";
	}
	else
	{
		m_documentXslt = file.readAll();
	}
}

Documentor::~Documentor()
{
}


bool Documentor::openFile(const QCoreApplication& app)
{
	return m_fileManager.openArgumentFilename(app);
}

void Documentor::contentChanged()
{
	m_timer.stop();
	m_fileManager.setIsSaved(false);

	auto now = QTime::currentTime();
	if (std::abs(m_lastUpdatedTime.msecsTo(now)) > s_msDelayToUpdate ) //abs to tolerate system clock changes
	{
		onUpdateTimeout();
	}
	else
	{
		m_timer.start();
	}
}

void Documentor::closeEvent(QCloseEvent *event)
{
	auto ret = m_fileManager.onCloseFile();
	if (ret)
	{
		QApplication::quit();
	}
	else
	{
		event->setAccepted(false);
	}
}

QString Documentor::headerCounting( const QString& inputStr)
{
	auto out = inputStr;
	constexpr std::string_view s_tag = "8dl912_numberKey";
	
	// xsl::number is not implemented in QXmlQuery, thus, the counting of headers
	//    1 Header
	//    1.1 SubHeader
	//    2 Header2
	// is not possible. To improve this, each "index" is replaced by a fixed string
	//     8dl912_numberKey1 for headers of level 1, 8dl912_numberKey2 for headers of level 2
	// and so on. 
	// This function replace those strings with an auto-incremental counter.
	
	// Return the position in the string of the next "tag"
	auto getNextTagPos = [&s_tag](const QString& out, int pos)->int 
	{
		if (pos >= out.size()) return -1;
		return out.indexOf( s_tag.data(), pos); 
	};
	
	// Get the next digit after the tag, to return which header level it is.
	auto getLevel = [&s_tag](const QString& out, int pos)->int
	{
		auto ref = out.midRef(pos+s_tag.size(), 1); //Only one digit level
		bool conversionSuccess = false;
		auto level = ref.toInt(&conversionSuccess);
		if (conversionSuccess)
		{
			return level;
		}
		return -1;
	};
	
	// counters
	int c1=0;
	int c2=0;
	int c3=0;
	
	for (int pos = getNextTagPos(out, 0); pos >= 0; pos = getNextTagPos(out, pos))
	{
		auto level = getLevel(out, pos);
		if (1==level)
		{
			out = out.replace(pos,17, QString::number(++c1));
			c2=1;
			c3=1;
		}
		else if(2==level)
		{
			out = out.replace(pos,17, QString::number(c1) + "." + QString::number(++c2));
			c3=1;
		}
		else if(3==level)
		{
			out = out.replace(pos,17, QString::number(c1) + "." + 
			    QString::number(c2) + "." + QString::number(++c3));
		}
		pos++;
	} 
	return out;
}

void Documentor::onUpdateTimeout()
{
	QString out;
	
	constexpr std::string_view s_emptyPage = "<!DOCTYPE html><html><head><title>Empty</title>"
	    "</head><body style='background-color: rgb(224,224,224);'></body></html>";
	constexpr std::string_view s_errorPage = "<!DOCTYPE html><html><head><title>Error</title>"
	    "</head><body style='background-color: rgb(255,224,224);'></body></html>";
	
	if (m_uiCenterPtr->textEditor->document()->characterCount() < 4)
	{
		// Empty or nearly empty file, don't try to parse it.
		out = s_emptyPage.data();
	}
	else
	{
		auto fret = m_query.setFocus( m_uiCenterPtr->textEditor->toPlainText());
		if (!fret)
		{
			// Error in XML content
			out = s_errorPage.data();
		}
		else
		{
			m_query.setQuery(m_documentXslt);
			m_query.evaluateTo(&out);
		}
	}
	
	// For some reason, QXmlQuery does not add the doctype, append it
	out = "<!DOCTYPE html>\n"  + out;

	// xsl:number no implemented, use keys to implement it
	out = headerCounting(out);

	if (m_fileManager.getCurrentlyOpenPath().empty())
	{
		m_uiCenterPtr->browser->setContent(QByteArray::fromStdString(out.toStdString()),"text/html", 
		    QUrl::fromLocalFile(QFileInfo("~/").absolutePath()));
	}
	else
	{
		QFileInfo curFile(m_fileManager.getCurrentlyOpenPath().c_str());
		m_uiCenterPtr->browser->setContent(QByteArray::fromStdString(out.toStdString()),"text/html", 
		    QUrl::fromLocalFile(curFile.absoluteDir().absolutePath() + QDir::separator()));
	}
	
	if constexpr (OUTPUT_TO_FILE) // for debuging
	{
		std::ofstream of("./index.html", std::ios::out | std::ios::binary);
		of << out.toStdString();
		of.close();
	}
	
	m_lastUpdatedTime = QTime::currentTime();
}

void Documentor::setupMenu()
{
	QMenu *menu = menuBar()->addMenu(tr("&File"));

	menu->addAction(tr("New"), &m_fileManager, SLOT(onNewFile()))->setShortcut(tr("Ctrl+N"));
	menu->addAction(tr("Open"), &m_fileManager, SLOT(onOpenFile()))->setShortcut(tr("Ctrl+O"));
	menu->addAction(tr("Save"), &m_fileManager, SLOT(onSaveFile()))->setShortcut(tr("Ctrl+S"));
	menu->addAction(tr("Save as"), &m_fileManager, SLOT(onSaveAsFile()))->setShortcut(tr("Ctrl+Shift+S"));
	menu->addAction(tr("Close"), &m_fileManager, SLOT(onCloseFile()))->setShortcut(tr("Ctrl+W"));

	menu->addSeparator();
	menu->addAction(tr("Quit"), [this]()
	{
		auto ret = m_fileManager.onCloseFile();
		if (ret)
		{
			QApplication::quit();
		}
	})->setShortcut(tr("Ctrl+Q"));
	
	QMenu *pdfMenu = menuBar()->addMenu(tr("&Pdf"));
	pdfMenu->addAction(tr("Export PDF"), this, [this]()
	{
		FileManager::exportFilename( [this]( std::ofstream& os)->bool
		{ return m_uiCenterPtr->browser->printToPdf(os); }, "PDF (*.pdf)");
	})->setShortcut(tr("Ctrl+E"));
}

}

//#include "Documentor.moc"
