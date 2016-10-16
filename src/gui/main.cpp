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
#include "dindexer-guiConfig.h"
#include "dindexer-common/settings.hpp"
#include "icon_provider.hpp"
#include "result_list_model.hpp"
#include <QApplication>
#include <QtCore/QUrl>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <cassert>
#include <iostream>

//namespace {
//	std::string replace_dindexer_path (const char* parPath, const std::map<std::string, std::string>& parDict) a_pure;
//
//	std::string replace_dindexer_path (const char* parPath, const std::map<std::string, std::string>& parDict) {
//		assert(parPath);
//		std::string retval(parPath);
//
//		std::size_t from = 0;
//		while ((from = retval.find('%', from)) != std::string::npos) {
//			if (retval.size() - 1 == from)
//				break;
//			if ('%' == retval[from + 1]) {
//				from += 2;
//				continue;
//			}
//
//			const auto to = retval.find('%', from + 1);
//			if (std::string::npos == to)
//				break;
//
//			assert(to - from + 1 > 2);
//			const auto& val = parDict.at(retval.substr(from + 1, to - from - 1));
//			retval.replace(from, to - from + 1, val);
//		}
//		if (not retval.empty() and retval.back() != '/')
//			retval += '/';
//		return retval;
//	}
//} //unnamed namespace

int main (int parArgc, char* parArgv[]) {
	QApplication app(parArgc, parArgv);
	QQmlApplicationEngine engine;
	din::ResultListModel result_list_model;

	{
		QQmlContext* ctxt = engine.rootContext();
		assert(ctxt);
		ctxt->setContextProperty("resultListModel", &result_list_model);
	}

	engine.addImageProvider(QLatin1String("theme"), new din::IconProvider);
	//engine.load(QUrl::fromLocalFile(QString::fromUtf8(qml_path.c_str(), qml_path.size())));
	engine.load(QUrl::fromLocalFile(QML_PATH "/mainwin.qml"));

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
