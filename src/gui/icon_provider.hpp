/* Copyright 2015, 2016, Michele Santullo
 * This file is part of "dindexer".
 *
 * "dindexer" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "dindexer" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "dindexer".  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef idFE04F9FAD3F64CABB69A9CC620FB731E
#define idFE04F9FAD3F64CABB69A9CC620FB731E

#include <QQuickImageProvider>

namespace din {
	class IconProvider : public QQuickImageProvider {
	public:
		IconProvider ( void );

		QPixmap requestPixmap (const QString& parID, QSize* parSize, const QSize& parRequestedSize) override;
	};
} //namespace din

#endif
