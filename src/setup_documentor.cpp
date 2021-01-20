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

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QWebEngineView>

#include "ui_center.h"

namespace Escain
{
void Documentor::setupGui()
{
	m_uiCenterPtr = std::make_unique<Ui::documentorCenter>();
	
	// Main splitter resizing factors
	m_uiCenterPtr->setupUi(&m_center);
	m_uiCenterPtr->horizontalSplitter->setStretchFactor(0, 1);
	m_uiCenterPtr->horizontalSplitter->setStretchFactor(1, 1);
	m_uiCenterPtr->horizontalSplitter->setSizes(QList<int>({50, 50}));
	
	// Web engine settings
	QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
	QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, false);
	QWebEngineProfile::defaultProfile()->setUseForGlobalCertificateVerification();
	
	// Set font for the editor
	QFont font = m_uiCenterPtr->textEditor->font();
	font.setFixedPitch(true);
	font.setFamily("Courier");
	m_uiCenterPtr->textEditor->setFont(font);
	
	// Set tab size
	QFontMetrics fontMetrics(font);
	int spaceWidth = fontMetrics.horizontalAdvance(' ');
	m_uiCenterPtr->textEditor->setTabStopDistance(spaceWidth * 4);
}
}
