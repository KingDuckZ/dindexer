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

#include "dindexer-machinery/guess_content_type.hpp"
#include "dindexer-machinery/set_listing.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <vector>

namespace {
	template <std::size_t N>
	void detect_type (mchlib::FileRecordData (&parTestData)[N], mchlib::ContentTypes parExpected, mchlib::MediaTypes parMediaType) {
		using mchlib::SetListing;
		using mchlib::FileRecordData;

		std::vector<FileRecordData> test_data(
			std::make_move_iterator(std::begin(parTestData)),
			std::make_move_iterator(std::end(parTestData))
		);
		SetListing dvd_listing(std::move(test_data));

		const mchlib::ContentTypes detected_type =
			guess_content_type(parMediaType, dvd_listing.make_cview());

		EXPECT_EQ(parExpected, detected_type);
	}
} //unnamed namespace

TEST(machinery, guess_content_type) {
	using mchlib::FileRecordData;

	{
		FileRecordData test_data[] = {
			FileRecordData("",0,0,0,true,false),
			FileRecordData("VIDEO_TS",0,0,1,true,false),
			FileRecordData("AUDIO_TS",0,0,1,true,false),
			FileRecordData("VIDEO_TS/VTS_01_0.BUP",0,0,2,false,false),
			FileRecordData("VIDEO_TS/VTS_01_0.VOB",0,0,2,false,false),
			FileRecordData("VIDEO_TS/VIDEO_TS.VOB",0,0,2,false,false)
		};
		detect_type(test_data, mchlib::ContentType_VideoDVD, mchlib::MediaTypes::DVD);
	}

	{
		FileRecordData test_data[] = {
			FileRecordData("",0,0,0,true,false),
			FileRecordData("dummy",0,0,1,true,false),
			FileRecordData("another_dir",0,0,1,true,false),
			FileRecordData("some_file.bin",0,0,1,false,false),
			FileRecordData("another_dir/VTS_01_0.BUP",0,0,2,false,false)
		};
		detect_type(test_data, mchlib::ContentType_Generic, mchlib::MediaTypes::Directory);
	}

	{
		FileRecordData test_data[] = {
			FileRecordData("",0,0,0,true,false),
			FileRecordData("SVCD",0,0,1,true,false),
			FileRecordData("SVCD/INFO.VCD",0,0,2,false,false),
			FileRecordData("SVCD/ENTRIES.VCD",0,0,2,false,false),
			FileRecordData("SVCD/SEARCH.DAT",0,0,2,false,false),
			FileRecordData("SVCD/PSD.VCD",0,0,2,false,false),
			FileRecordData("MPEGAV",0,0,1,true,false),
			FileRecordData("MPEGAV/AVSEQ01.DAT",0,0,2,false,false),
			FileRecordData("SEGMENT",0,0,1,true,false),
			FileRecordData("SEGMENT/ITEM001.DAT",0,0,2,false,false),
			FileRecordData("CDI",0,0,1,true,false),
			FileRecordData("KARAOKE",0,0,1,true,false)
		};
		detect_type(test_data, mchlib::ContentType_VideoCD, mchlib::MediaTypes::CDRom);
	}
}
