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

#include "icon_provider.hpp"
#include <QIcon>
#include <QMimeType>
#include <QMimeDatabase>

namespace din {
	IconProvider::IconProvider() :
		QQuickImageProvider(QQuickImageProvider::Pixmap)
	{
	}

	QPixmap IconProvider::requestPixmap (const QString& parID, QSize* parSize, const QSize& parRequestedSize) {
		const int width = (parRequestedSize.width() <= 0 ? 64 : parRequestedSize.width());
		const int height = (parRequestedSize.height() <= 0 ? 64 : parRequestedSize.height());

		QMimeDatabase mime_db;
		QMimeType mime_type = mime_db.mimeTypeForName(parID);
		QIcon icon(QIcon::fromTheme(mime_type.iconName()));
		const QSize icon_size = icon.actualSize(QSize(width, height));

		if (parSize)
			*parSize = icon_size;

		return icon.pixmap(icon_size);
	}
} //namespace din
