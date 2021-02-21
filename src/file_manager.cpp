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

#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

namespace Escain
{

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
		
		// If full URL is specificed, remove scheme "File::".
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

bool FileManager::unableToSave() const
{
	QMessageBox msg;
	msg.setText(tr("I could not save in the current file. Sorry!"));
	msg.setStandardButtons( QMessageBox::Ok |QMessageBox::Cancel);
	msg.setDefaultButton( QMessageBox::Ok);
	return ( msg.exec()==QMessageBox::Ok);
}

bool FileManager::onSaveAsFile()
{
	bool result = false;
	bool stopTrying = false;

	do
	{
		QFileDialog dialog;

		dialog.setFileMode( QFileDialog::AnyFile );
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		dialog.setNameFilter(QString::fromStdString(m_fileFilter));
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
				m_currentOpenFile = nUrl.url().toStdString();
				result = emit extractContentTo( fileStream);
				stopTrying=true; // do not try again
			}
			else if (!unableToSave())
			{
				stopTrying = true;
			}
		}
	} while( !stopTrying );

	setIsSaved(result);

	return result;
}


bool FileManager::onCloseFile()
{
	if (m_saved)
	{
		return true;
	}

	QMessageBox msg;
	msg.setText(tr("Do you want to save this document?"));
	msg.setStandardButtons( QMessageBox::Yes |QMessageBox::No | QMessageBox::Cancel);
	msg.setDefaultButton( QMessageBox::Yes);

	int ret = msg.exec();
	if (ret==QMessageBox::Yes) return onSaveFile();
	else if (ret==QMessageBox::Cancel) return false;

	return true;
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
		// No need to save the current file
		setIsSaved(true);
		// Empty current content
		emit clearContent();
	}

	return createNew;
}

}
