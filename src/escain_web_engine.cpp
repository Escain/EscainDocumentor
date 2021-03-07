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

#include "escain_web_engine.hpp"


#include <QAction>
#include <QByteArray>
#include <QDebug>
#include <QEventLoop>
#include <QWebEngineCallback>
#include <QWheelEvent>

namespace Escain
{

EscainWebEngine::EscainWebEngine(QWidget* parent): QWebEngineView( parent )
{
	setZoomFactor(getZoomAt(m_zoomPos));
	connect( this->page(), &QWebEnginePage::pdfPrintingFinished, [](){ qDebug() << "PDF generated."; });
}

bool EscainWebEngine::printToPdf(std::ofstream& os)
{
	QPageLayout layout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF());
	layout.setUnits(QPageLayout::Millimeter);
	layout.setMargins(QMarginsF(0,20,0,20));
	layout.setMode(QPageLayout::Mode::FullPageMode);
	qDebug() << "Generating PDF";
	
	bool ret=false;
	QEventLoop loop;
	const QWebEngineCallback<const QByteArray&> webEngineCallback = 
	[&os, &ret, &loop](const QByteArray& arrayResult)
	{
		if (!arrayResult.isEmpty())
		{
			os << arrayResult.toStdString();
			ret = true;
		}
		loop.quit();
	};
	
	// Getting synchronous instead of asynchronous, 
	// because we want to loop asking the user for new filename if saving fails
	page()->printToPdf(webEngineCallback, layout);
	loop.exec();
	
	return ret;
}
		
const std::array<double,28>& EscainWebEngine::getAvailableZooms() const
{
	static const std::array<double,28> zooms {30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 
			110.0, 120.0, 130.0, 140.0, 150.0, 160.0, 170.0, 180.0, 190.0, 200.0, 210.0, 
			220.0, 230.0, 240.0, 250.0, 260.0, 270.0, 280.0, 290.0, 300.0};
	
	return zooms;
}

double EscainWebEngine::getZoomAt( size_t pos) const
{
	const auto& zooms = getAvailableZooms();
	
	if (pos > zooms.size())
	{
		pos = zooms.size()-1;
	}
	
	return zooms[pos]/100.0;
}

void EscainWebEngine::setZoom( double zoom )
{
	const auto& zoomsVect = getAvailableZooms();
	
	if (zoomsVect.size() > 0)
	{
		if (zoom < zoomsVect[0])
		{
			zoom = zoomsVect[0];
		}
		
		if (zoom > zoomsVect[zoomsVect.size()-1])
		{
			zoom = zoomsVect[zoomsVect.size()-1];
		}
	}
	
	setZoomFactor(zoom);
}

void EscainWebEngine::wheelEvent( QWheelEvent* event)
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
		
		setZoom(getZoomAt(m_zoomPos));
		event->accept();
		
		return;
	}
	
	// Ignore other events
	event->ignore();
}

}
