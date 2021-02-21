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

#ifndef Documentor_H
#define Documentor_H

#include <memory>

#include <QTime>
#include <QTimer>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QXmlQuery>

#include "file_manager.hpp"

namespace Ui
{
	class documentorCenter;
}

namespace Escain
{
	class Documentor : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit Documentor(QWidget *parent = nullptr);
		virtual ~Documentor();

	public slots:
		/// Called periodically to update the Web View with editor content
		void onUpdateTimeout();
		
		static QString headerCounting( const QString& ); // implement xsl::number, not present in QXmlQuery
		
	private:
		/// Override to manage saving before to close
		void closeEvent(QCloseEvent *event);
		
		void contentChanged(); // manage changes in the editor content

		QWidget m_center; // UI for the center view
		FileManager m_fileManager; // Management of file open/save/...
		QTimer m_timer; // Call periodically to update the Web View
		QXmlQuery m_query; // XSLT processing
		std::unique_ptr<Ui::documentorCenter> m_uiCenterPtr; //UI for center view (forward declared)
		QTime m_lastUpdatedTime; //last timestamp the web view was updated

		virtual void setupMenu();
		virtual void setupGui();
		
		constexpr static int s_msDelayToUpdate = 100; // Delay to update the view with editor content
	};
}

#endif // Documentor_H
