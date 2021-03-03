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

#ifndef EscainWebEngine_H
#define EscainWebEngine_H

#include <array>
#include <qwebengineview.h>
#include <QWheelEvent>
#include <QDebug>
#include <QAction>

namespace Escain
{
	class EscainWebEngine : public QWebEngineView
	{
		Q_OBJECT
	public:
		explicit EscainWebEngine(QWidget* parent=nullptr): QWebEngineView( parent )
		{
			setZoomFactor(m_zoomPos);
		}
		virtual ~EscainWebEngine() = default;
	private:
	
		
		const std::array<double,28>& getAvailableZooms() const
		{
			static const std::array<double,28> zooms {30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 
				    110.0, 120.0, 130.0, 140.0, 150.0, 160.0, 170.0, 180.0, 190.0, 200.0, 210.0, 
				    220.0, 230.0, 240.0, 250.0, 260.0, 270.0, 280.0, 290.0, 300.0};
			
			return zooms;
		}
		void setZoom( size_t pos)
		{
			const auto& zooms = getAvailableZooms();
			
			if (pos > zooms.size())
			{
				pos = zooms.size()-1;
			}
			
			setZoomFactor(zooms[pos]/100.0);
		}
		void wheelEvent( QWheelEvent* event)
		{
			if(event->modifiers().testFlag(Qt::ControlModifier))
			{
				const auto& zooms = getAvailableZooms();
				if (event->angleDelta().y() > 0.1 && m_zoomPos+1 < zooms.size())
				{
					m_zoomPos++;
				}
				else if(event -> angleDelta().y() < 0.1 && m_zoomPos>0)
				{
					m_zoomPos--;
				}
				
				setZoom(m_zoomPos);
				event->accept();
				
				return;
			}
			
			// Ignore other events
			event->ignore();
		}
		
		size_t m_zoomPos = 7;
	};
}

#endif // EscainWebEngine_H
 
