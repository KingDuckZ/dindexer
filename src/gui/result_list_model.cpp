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

#include "result_list_model.hpp"
#include <ciso646>

namespace din {
	ResultListModel::ResultListModel (QObject* parParent) :
		QAbstractListModel(parParent)
	{
	}

	int ResultListModel::rowCount (const QModelIndex& parParent) const {
		if (parParent.isValid())
			return 0;
		return 3;
	}

	QVariant ResultListModel::data (const QModelIndex& parIndex, int parRole) const {
		if (not parIndex.isValid())
			return QVariant();

		switch (parRole) {
		case Qt::DisplayRole:
			return QString("Dummy text");

		case Qt::DecorationRole:
			return QString("image://theme/audio/mp4");

		default:
			return QVariant();
		}
	}
} //namespace din
