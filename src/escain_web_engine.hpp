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
#include <fstream>
#include <qwebengineview.h>

class QWheelEvent;

namespace Escain
{

/// This class extens the qwebengineview adding features needed for this
///  project, like mouse-wheel zooming, pdf export, etc.
class EscainWebEngine : public QWebEngineView
{
	Q_OBJECT
public:
	explicit EscainWebEngine(QWidget* parent=nullptr);
	virtual ~EscainWebEngine() = default;
	
	/// Produce a PDF from the current document, for exporting
	/// return if the export could be done
	bool printToPdf( std::ofstream& filename);
private:

	/// Get the list of available zooms, as a proportion: 1.0, 1.2 (120%)...
	const std::array<double,28>& getAvailableZooms() const;
	/// Retrieve the zoom at specific pos in available zooms.
	double getZoomAt( size_t pos) const;
	/// Set a specific zoom
	void setZoom( double zoom);
	/// Override the wheel event
	void wheelEvent( QWheelEvent* event);
	
	size_t m_zoomPos = 7;
};

}

#endif // EscainWebEngine_H
 
