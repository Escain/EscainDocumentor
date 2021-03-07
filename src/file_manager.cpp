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

#include "file_manager.hpp"

#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

namespace Escain
{

FileManager::FileManager(QObject* parent)
    : QObject(parent)
{
}

bool FileManager::onOpenFile(const std::string& file)
{
	// Refresh to an empty document, saving the previous if required
	bool isNew = onNewFile();

	if (isNew)
	{
		QUrl nUrl;
		if (file.length()==0)
		{
			QFileDialog dialog;

			dialog.setFileMode( QFileDialog::ExistingFile );
			dialog.setAcceptMode(QFileDialog::AcceptOpen);
			dialog.setNameFilter(QString::fromStdString(m_fileFilter));
			if(!dialog.exec() || dialog.selectedUrls().size()==0)
			{
				return false;
			}
			QUrl url= dialog.selectedUrls()[0];
			nUrl = url.toLocalFile();
		}
		else
		{
			nUrl.setUrl( QString::fromStdString(file) );
		}
		
		std::ifstream fileStream;
		fileStream.open(nUrl.url().toStdString() , std::ios::in | std::ios::binary);

		if (fileStream)
		{
			m_currentOpenFile = nUrl.url().toStdString();
			emit loadContentFrom(fileStream);
			setIsSaved(true);
			return true;
		}
	}
	
	return false;
}

bool FileManager::onSaveFile()
{
	bool result = false;
	if (m_currentOpenFile.empty())
	{
		result = onSaveAsFile();
	}
	else
	{

		std::ofstream file;
		file.open(m_currentOpenFile , std::ios::out | std::ios::binary);

		if (file)
		{
			result = emit extractContentTo( file );
		}
		else
		{
			if (unableToSave())
			{
				result = onSaveAsFile();
			}
		}
	}
	setIsSaved(result);
	return result;
}

bool FileManager::unableToSave()
{
	QMessageBox msg;
	msg.setText(tr("I could not save in the current file. Sorry!"));
	msg.setStandardButtons( QMessageBox::Ok |QMessageBox::Cancel);
	msg.setDefaultButton( QMessageBox::Ok);
	return ( msg.exec()==QMessageBox::Ok);
}

bool FileManager::onSaveAsFile()
{

	auto result = exportFilename( [this]( std::ofstream& of)->bool{ return extractContentTo(of);} , m_fileFilter);
	
	if (!result.has_value())
	{
		return false;
	}
	
	m_currentOpenFile = *result;
	return true;
}


bool FileManager::onCloseFile()
{
	bool closeFile = false;
	if (m_saved)
	{
		closeFile = true;
	}
	else
	{
		QMessageBox msg;
		msg.setText(tr("Do you want to save this document?"));
		msg.setStandardButtons( QMessageBox::Yes |QMessageBox::No | QMessageBox::Cancel);
		msg.setDefaultButton( QMessageBox::Yes);

		int ret = msg.exec();
		if (ret==QMessageBox::Yes)
		{
			closeFile = onSaveFile();
		}
		else if (ret==QMessageBox::Cancel)
		{
			closeFile = false;
		}
		else
		{
			closeFile = true;
		}
	}

	if (closeFile)
	{
		// Make no file is the current
		m_currentOpenFile = "";
		// Empty current content
		emit clearContent();
		// No need to save the current file
		setIsSaved(true);
	}

	return closeFile;
}


void FileManager::setIsSaved( bool isSaved )
{
	m_saved = isSaved;

	QString title = tr("new document");
	
	if (!m_currentOpenFile.empty())
	{
		QUrl url(QString::fromStdString(m_currentOpenFile));
		title = url.fileName();
	}
	
	if (!isSaved)
	{
		title = tr("* ") + title;
	}
	
	emit setFileTitle( title.toStdString() );
}


bool FileManager::onNewFile()
{
	bool createNew = false;
	if (m_saved)
	{
		createNew = true;
	}
	else
	{
		QMessageBox msg;
		msg.setText(tr("Do you want to save this document?"));
		msg.setStandardButtons( QMessageBox::Yes |QMessageBox::No | QMessageBox::Cancel);
		msg.setDefaultButton( QMessageBox::Yes);

		int ret = msg.exec();
		if (ret==QMessageBox::Yes)
		{
			createNew = onSaveFile();
		}
		else if (ret==QMessageBox::Cancel)
		{
			createNew = false;
		}
		else
		{
			createNew = true;
		}
	}

	if (createNew)
	{
		// Make no file is the current
		m_currentOpenFile = "";
		// Empty current content
		emit clearContent();
		// No need to save the current file
		setIsSaved(true);
	}

	return createNew;
}

bool FileManager::openArgumentFilename(const QCoreApplication& app)
{
	const auto args = app.arguments();
	for (int i=0; i<args.size(); ++i)
	{
		if (0==i) continue; // skip application path
		
		QUrl nUrl( args[i] );
		
		std::ifstream fileStream;
		fileStream.open(nUrl.url().toStdString() , std::ios::in | std::ios::binary);
	
		if (fileStream)
		{
			m_currentOpenFile = nUrl.url().toStdString();
			emit loadContentFrom(fileStream);
			setIsSaved(true);
			return true;
		}
	}
	
	// clear the page, to a new document.
	emit clearContent();
	// No need to save the current file
	setIsSaved(true);
	
	return false;
}

std::optional<std::string> FileManager::exportFilename( const ExtractCallback& callback, const std::string& filter)
{
	bool result = false;
	bool stopTrying = false;
	std::string fileName;

	do
	{
		QFileDialog dialog;

		dialog.setFileMode( QFileDialog::AnyFile );
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		dialog.setNameFilter(QString::fromStdString(filter));
		if(!dialog.exec() || dialog.selectedUrls().size()==0)
		{
			result = false; // not saved
			stopTrying = true; // do not try again
		}
		else
		{
			QUrl url= dialog.selectedUrls()[0];
			QUrl nUrl = url.toLocalFile();

			std::ofstream fileStream;
			fileStream.open(nUrl.url().toStdString() , std::ios::out | std::ios::binary);

			if (fileStream)
			{
				fileName = nUrl.url().toStdString();
				result = emit callback( fileStream);
				stopTrying=true; // do not try again
			}
			else if (!unableToSave())
			{
				stopTrying = true;
			}
		}
	} while( !stopTrying );

	return result ? std::optional(fileName) : std::nullopt;
}

}
