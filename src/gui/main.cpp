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

#include "searcher.hpp"
#include "dindexerConfig.h"
#include "dindexer-common/settings.hpp"
#include <QApplication>
#include <QtCore/QUrl>
#include <QQmlApplicationEngine>
#include <cassert>
#include <iostream>

int main (int parArgc, char* parArgv[]) {
	QApplication app(parArgc, parArgv);
	QQmlApplicationEngine engine;
	engine.load(QUrl::fromLocalFile("/home/michele/dev/code/cpp/dindexer/src/gui/qml/mainwin.qml"));

	dinlib::Settings settings;
	try {
		dinlib::load_settings(CONFIG_FILE_PATH, settings);
	}
	catch (const std::runtime_error& err) {
		std::cerr << "Can't load settings from " << CONFIG_FILE_PATH << ":\n";
		std::cerr << err.what() << '\n';
		return 1;
	}

	QObject* const obj = engine.rootObjects()[0];
	assert(obj);
	din::Searcher searcher(settings.backend_plugin.backend());
	QObject::connect(obj, SIGNAL(locate(const QString&)), &searcher, SLOT(on_locate(const QString&)));
	return app.exec();
}
