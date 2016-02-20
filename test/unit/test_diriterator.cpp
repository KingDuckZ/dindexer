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

#include "dindexer-machinery/set_listing.hpp"
#include "dindexer-machinery/set_listing_helpers.hpp"
#include "helpers/lengthof.h"
#include <gtest/gtest.h>
#include <vector>
#include <utility>
#include <algorithm>

//TEST_F for class

namespace {
} //unnamed namespace

TEST(machinery, diriterator) {
	using mchlib::SetListing;
	using mchlib::SetListingView;
	using mchlib::FileRecordData;

	FileRecordData test_data_arr[] = {
		FileRecordData("",0,0,0,true,false),
		FileRecordData("BestAndBest",0,0,1,true,false),
		FileRecordData("BestAndBest/CD1",0,0,2,true,false),
		FileRecordData("BestAndBest/CD1/01 - 紅三四郎.flac",0,0,3,false,false),
		FileRecordData("BestAndBest/CD1/堀江美都子.-.[BEST.And.BEST.Disc1].专辑.(ape).cue",0,0,3,false,false),
		FileRecordData("BestAndBest/CD2",0,0,2,true,false),
		FileRecordData("BestAndBest/CD2/01 - ダルタニアスの歌.flac",0,0,3,false,false),
		FileRecordData("BestAndBest/CD2/02 - 花のなかの花.flac",0,0,3,false,false),
		FileRecordData("BestAndBest/CD2/堀江美都子.-.[BEST.And.BEST.Disc2].专辑.(ape).cue",0,0,3,false,false),
		FileRecordData("BestAndBest/CD.txt",0,0,2,false,false),
		FileRecordData("BestAndBest/Scans",0,0,2,true,false),
		FileRecordData("BestAndBest/Scans/01.jpg",0,0,3,false,false),
		FileRecordData("Cowboy Bebop - Tank - The Best",0,0,1,true,false),
		FileRecordData("Cowboy Bebop - Tank - The Best/01 - Tank! (TV stretch).mp3",0,0,2,false,false),
		FileRecordData("Escaflowne",0,0,1,true,false),
		FileRecordData("Escaflowne/cd1 - Over the sky",0,0,2,true,false),
		FileRecordData("Escaflowne/cd1 - Over the sky/01 - Yakusoku wa iranai.mp3",0,0,3,false,false),
		FileRecordData("Escaflowne/cd2",0,0,2,true,false),
		FileRecordData("Escaflowne/cd2/01 - The vision of Escaflowne.mp3",0,0,3,false,false),
		FileRecordData("Escaflowne/cd3",0,0,2,true,false),
		FileRecordData("Escaflowne/cd3/01 - Short notice.mp3",0,0,3,false,false),
		FileRecordData("Last Exile",0,0,1,true,false),
		FileRecordData("Last Exile/cd1",0,0,2,true,false),
		FileRecordData("Last Exile/cd2",0,0,2,true,false),
		FileRecordData("Ragnarok",0,0,1,true,false),
		FileRecordData("Tekkaman",0,0,1,true,false),
		FileRecordData("Tekkaman LP",0,0,1,true,false),
		FileRecordData("Tekkaman LP/01 - Ichirou Mizuki - Tekkaman no uta.mp3",0,0,2,false,false),
		FileRecordData("Tekkaman LP/02 - Ichirou Mizuki - Space Knight no uta.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5",0,0,1,true,false),
		FileRecordData("Vultus 5/voltes_v_ending_full_minus_one_karaoke.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_ending_full.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_ending_tv.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_opening_full_minus_one_karaoke.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_opening_full.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_opening_full_reperformed.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_opening_horie_mitsuko_live.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/voltes_v_opening_tv.mp3",0,0,2,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV",0,0,2,true,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/01_ボルテスVの歌 (TV用唄入り).mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/02_侵略の魔手 〜ボアザン星辺境征軍地球に来襲す〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/03_ボルテス・雄姿の出現 〜地球の命運を双肩に賭けて〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/04_若き英雄たち 〜正義に燃える青春の血潮〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/05_ブリッジ・コレクション.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/06_ボルテス・死闘の戦歴 〜壮絶なる超電磁の攻防戦〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/07_剛兄弟出生の秘密 〜ラ・ゴール如何に宿命と闘いしか〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/08_大宇宙への出発 〜悲劇のボアザン星解放へ向かって〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/09_悲劇の終幕 〜貴公子ハイネル紅蓮の炎に消ゆ〜.mp3",0,0,3,false,false),
		FileRecordData("Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/10_父をもとめて (TV用唄入り).mp3",0,0,3,false,false)
	};

	const char* const expected_list[] = {
		"BestAndBest",
		"BestAndBest/CD1",
		"BestAndBest/CD1/01 - 紅三四郎.flac",
		"BestAndBest/CD1/堀江美都子.-.[BEST.And.BEST.Disc1].专辑.(ape).cue",
		"BestAndBest/CD2",
		"BestAndBest/CD2/01 - ダルタニアスの歌.flac",
		"BestAndBest/CD2/02 - 花のなかの花.flac",
		"BestAndBest/CD2/堀江美都子.-.[BEST.And.BEST.Disc2].专辑.(ape).cue",
		"BestAndBest/Scans",
		"BestAndBest/Scans/01.jpg",
		"BestAndBest/CD.txt",
		"Cowboy Bebop - Tank - The Best",
		"Cowboy Bebop - Tank - The Best/01 - Tank! (TV stretch).mp3",
		"Escaflowne",
		"Escaflowne/cd1 - Over the sky",
		"Escaflowne/cd1 - Over the sky/01 - Yakusoku wa iranai.mp3",
		"Escaflowne/cd2",
		"Escaflowne/cd2/01 - The vision of Escaflowne.mp3",
		"Escaflowne/cd3",
		"Escaflowne/cd3/01 - Short notice.mp3",
		"Last Exile",
		"Last Exile/cd1",
		"Last Exile/cd2",
		"Ragnarok",
		"Tekkaman",
		"Tekkaman LP",
		"Tekkaman LP/01 - Ichirou Mizuki - Tekkaman no uta.mp3",
		"Tekkaman LP/02 - Ichirou Mizuki - Space Knight no uta.mp3",
		"Vultus 5",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/01_ボルテスVの歌 (TV用唄入り).mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/02_侵略の魔手 〜ボアザン星辺境征軍地球に来襲す〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/03_ボルテス・雄姿の出現 〜地球の命運を双肩に賭けて〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/04_若き英雄たち 〜正義に燃える青春の血潮〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/05_ブリッジ・コレクション.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/06_ボルテス・死闘の戦歴 〜壮絶なる超電磁の攻防戦〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/07_剛兄弟出生の秘密 〜ラ・ゴール如何に宿命と闘いしか〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/08_大宇宙への出発 〜悲劇のボアザン星解放へ向かって〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/09_悲劇の終幕 〜貴公子ハイネル紅蓮の炎に消ゆ〜.mp3",
		"Vultus 5/テレビオリジナルＢＧＭコレクション 超電磁マシーン ボルテスV/10_父をもとめて (TV用唄入り).mp3",
		"Vultus 5/voltes_v_ending_full.mp3",
		"Vultus 5/voltes_v_ending_full_minus_one_karaoke.mp3",
		"Vultus 5/voltes_v_ending_tv.mp3",
		"Vultus 5/voltes_v_opening_full.mp3",
		"Vultus 5/voltes_v_opening_full_minus_one_karaoke.mp3",
		"Vultus 5/voltes_v_opening_full_reperformed.mp3",
		"Vultus 5/voltes_v_opening_horie_mitsuko_live.mp3",
		"Vultus 5/voltes_v_opening_tv.mp3",
	};

	std::vector<FileRecordData> test_data(std::make_move_iterator(std::begin(test_data_arr)), std::make_move_iterator(std::end(test_data_arr)));
	SetListing lst(std::move(test_data));

	auto i = lst.begin();
	EXPECT_EQ("BestAndBest", i->abs_path);

	{
		auto view = SetListingView<true>(i);
		auto i2 = view.cbegin();
		EXPECT_EQ("BestAndBest/CD1", i2->abs_path);

		++i2;
		EXPECT_EQ("BestAndBest/CD2", i2->abs_path);

		++i2;
		EXPECT_EQ("BestAndBest/Scans", i2->abs_path);

		++i2;
		EXPECT_EQ("BestAndBest/CD.txt", i2->abs_path);

		++i2;
		EXPECT_TRUE(view.end() == i2);
	}

	++i;
	EXPECT_EQ("Cowboy Bebop - Tank - The Best", i->abs_path);

	std::vector<std::string> flattened;
	auto flattened_ptrs = mchlib::flattened_listing(lst.make_cview());
	flattened.reserve(lengthof(expected_list));
	for (const auto itm : flattened_ptrs) {
		flattened.push_back(itm->abs_path);
	}

	EXPECT_EQ(lengthof(expected_list), flattened.size());
	const auto count = std::min(lengthof(expected_list), flattened.size());
	for (std::size_t z = 0; z < count; ++z) {
		EXPECT_EQ(expected_list[z], flattened[z]);
	}
}
