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

#ifndef file_manager_h
#define file_manager_h

#include <fstream>
#include <string>
#include <QObject>

namespace Escain
{

/// Provide re-usable logic around open/save/close/save-as behaviours
class FileManager final: public QObject
{
	Q_OBJECT
public slots:
	/// The user requested to open a file (usually just pressed the button, so no path is provided)
	///     however, file path can be known (e.g. launching from the file explorer)
	/// @return bool: if the operation was successful (e.g. false if user cancel)
	bool onOpenFile(const std::string& file="");
	
	/// The user request to save the file
	/// @return bool: if the operation was successful (e.g. false if user cancel)
	bool onSaveFile();
	
	/// The user request to save-as the file
	/// @return bool: if the operation was successful (e.g. false if user cancel)
	bool onSaveAsFile();
	
	/// The user request to close the file (e.g. before to exit the application)
	/// @return bool: if the operation was successful (e.g. false if canceled because file not saved)
	bool onCloseFile();
	
	/// The user request a new file.
	/// @return bool: if the operation was successful (e.g. false if canceled because file not saved)
	bool onNewFile();
public:
	/// The filter allows to see in the FileDialogBox those file which can be open/saved
	void setFileExtensionFilter( const std::string& filter ){ m_fileFilter = filter; }
	std::string getFileExtensionFilter() const { return m_fileFilter; }
	
	/// Return the path to the file currently open. Empty if not saved/new
	std::string getCurrentlyOpenPath() const { return m_currentOpenFile; }
	
	/// Management of the status of saved/not saved file
	void setIsSaved( bool isSaved );
	/// Return if the current file is at a saved state
	bool getIsSaved() const { return m_saved; }
	
signals:
	/// Signal indicate to load the given file, and discard any previous content
	bool loadContentFrom( std::ifstream& file ) const;
	bool extractContentTo( std::ofstream& file ) const;
	void clearContent() const;
	
	void setFileTitle( const std::string& title );

private:
	/// Manage/messages when unable to save. Return if the user want to try again another file
	bool unableToSave() const;
	
	std::string m_currentOpenFile;
	std::string m_fileFilter = "XML (*.xml)";
	bool m_saved = true;
};


}
#endif //file_manager_h
