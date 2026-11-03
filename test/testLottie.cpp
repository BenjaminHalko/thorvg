/*
 * Copyright (c) 2024 - 2026 ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "config.h"
#include <thorvg.h>
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    #include <thorvg_lottie.h>
#endif
#include <fstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "catch.hpp"

using namespace tvg;
using namespace std;

#ifdef THORVG_LOTTIE_LOADER_SUPPORT

TEST_CASE("Lottie Coverages", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        #define TEST_CNT 12

        const char* names[TEST_CNT] = {
            "test3.lot",
            "test4.lot",
            "test5.lot",
            "test6.lot",
            "test7.lot",
            "test8.lot",
            "test9.lot",
            "test10.lot",
            "test11.lot",
            "test12.lot",
            "test13.lot",
            "test14.lot"
        };

        auto animation = unique_ptr<Animation>(Animation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        for (int i = 0; i < TEST_CNT; ++i) {
            char buf[100];
            snprintf(buf, sizeof(buf), TEST_DIR"/%s", names[i]);
            REQUIRE(picture->load(buf) == Result::Success);
            REQUIRE(animation->frame(0.0f) == Result::InsufficientCondition);
            REQUIRE(animation->frame(animation->totalFrame() * 0.5f) == Result::Success);
            REQUIRE(animation->frame(animation->totalFrame()) == Result::Success);
        }
    }
    REQUIRE(Initializer::term() == Result::Success);
}

TEST_CASE("Lottie Slot", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        //Slot Test 1
        const char* slotJson = R"({"gradient_fill":{"p":{"p":2,"k":{"a":0,"k":[0,0.1,0.1,0.2,1,1,0.1,0.2,0.1,1]}}}})";

        //Negative: slot generation before loaded
        REQUIRE(animation->gen(slotJson) == 0);

        REQUIRE(picture->load(TEST_DIR"/slot.lot") == Result::Success);

        auto id = animation->gen(slotJson);
        REQUIRE(id > 0);

        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id) == Result::Success);
        REQUIRE(animation->gen("") == 0);
        REQUIRE(animation->del(id) == Result::Success);

        //Slot Test 2
        const char* slotJson2 = R"({"lottie-icon-outline":{"p":{"a":0,"k":[1,1,0]}},"lottie-icon-solid":{"p":{"a":0,"k":[0,0,1]}}})";

        auto id2 = animation->gen(slotJson2);
        REQUIRE(id2 > 0);

        REQUIRE(animation->apply(id2) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id2) == Result::Success);
        REQUIRE(animation->del(id2) == Result::Success);

        //Slot Test 3 (Transform)
        const char* positionSlot = R"({"transform_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[100,100],"t":0},{"s":[200,300],"t":100}]}}})";
        auto id3 = animation->gen(positionSlot);
        REQUIRE(id3 > 0);
        REQUIRE(animation->apply(id3) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id3) == Result::Success);

        const char* scaleSlot = R"({"transform_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0,0],"t":0},{"s":[100,100],"t":100}]}}})";
        auto id4 = animation->gen(scaleSlot);
        REQUIRE(id4 > 0);
        REQUIRE(animation->apply(id4) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id4) == Result::Success);

        const char* rotationSlot = R"({"transform_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0],"t":0},{"s":[180],"t":100}]}}})";
        auto id5 = animation->gen(rotationSlot);
        REQUIRE(id5 > 0);
        REQUIRE(animation->apply(id5) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id5) == Result::Success);

        const char* opacitySlot = R"({"transform_id":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0],"t":0},{"s":[100],"t":100}]}}})";
        auto id6 = animation->gen(opacitySlot);
        REQUIRE(id6 > 0);
        REQUIRE(animation->apply(id6) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id6) == Result::Success);

        //Slot Test 4: Expression
        const char* expressionSlot = R"({"rect_rotation":{"p":{"x":"var $bm_rt = time * 360;"}},"rect_scale":{"p":{"x":"var $bm_rt = [];$bm_rt[0] = value[0] + Math.cos(2 * Math.PI * time) * 100;$bm_rt[1] = value[1];"}},"rect_position":{"p":{"x":"var $bm_rt = [];$bm_rt[0] = value[0] + Math.cos(2 * Math.PI * time) * 100;$bm_rt[1] = value[1];"}}})";
        auto id7 = animation->gen(expressionSlot);
        REQUIRE(id7 > 0);
        REQUIRE(animation->apply(id7) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id7) == Result::Success);

        //Slot Test 5: Text
        const char* textSlot = R"({"text_doc":{"p":{"k":[{"s":{"f":"Ubuntu Light Italic","t":"ThorVG!","j":0,"s":48,"fc":[1,1,1]},"t":0}]}}})";
        auto id8 = animation->gen(textSlot);
        REQUIRE(id8 > 0);
        REQUIRE(animation->apply(id8) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id8) == Result::Success);

        //Slot Test 6: Image
        const char* imageSlot = R"({"path_img":{"p":{"id":"image_0","w":200,"h":300,"u":"images/","p":"logo.png","e":0}}})";
        auto id9 = animation->gen(imageSlot);
        REQUIRE(id9 > 0);
        REQUIRE(animation->apply(id9) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id9) == Result::Success);

        //Slot Test 7: Time Remap
        const char* timeRemapSlot = R"({"time_remap":{"p":{"a":0,"k":1.5}}})";
        auto id10 = animation->gen(timeRemapSlot);
        REQUIRE(id10 > 0);
        REQUIRE(animation->apply(id10) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id10) == Result::Success);

        const char* timeRemapAnimatedSlot = R"({"time_remap":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[3],"t":0},{"s":[0],"t":180}]}}})";
        auto id11 = animation->gen(timeRemapAnimatedSlot);
        REQUIRE(id11 > 0);
        REQUIRE(animation->apply(id11) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id11) == Result::Success);

        //Slot Test 8: Rectangle
        const char* rectSlot = R"({"rectangle_position":{"p":{"a":0,"k":[400,400]}},"rectangle_size":{"p":{"a":0,"k":[200,120]}},"rectangle_radius":{"p":{"a":0,"k":30}}})";
        auto id12 = animation->gen(rectSlot);
        REQUIRE(id12 > 0);
        REQUIRE(animation->apply(id12) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id12) == Result::Success);
        REQUIRE(animation->del(id12) == Result::Success);

        const char* rectAnimatedSlot = R"({"rectangle_radius":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0],"t":0},{"s":[60],"t":100}]}}})";
        auto id13 = animation->gen(rectAnimatedSlot);
        REQUIRE(id13 > 0);
        REQUIRE(animation->apply(id13) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id13) == Result::Success);

        //Slot Test 9: Trim Path
        const char* trimpathSlot = R"({"trim_start":{"p":{"a":0,"k":25}},"trim_end":{"p":{"a":0,"k":100}},"trim_offset":{"p":{"a":0,"k":90}}})";
        auto id14 = animation->gen(trimpathSlot);
        REQUIRE(id14 > 0);
        REQUIRE(animation->apply(id14) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id14) == Result::Success);
        REQUIRE(animation->del(id14) == Result::Success);

        const char* trimpathAnimatedSlot = R"({"trim_end":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[0],"t":0},{"s":[100],"t":100}]}}})";
        auto id15 = animation->gen(trimpathAnimatedSlot);
        REQUIRE(id15 > 0);
        REQUIRE(animation->apply(id15) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id15) == Result::Success);

        //Slot Test 10: Stroke Width
        const char* strokeWidthSlot = R"({"stroke_width":{"p":{"a":0,"k":10}},"gradient_stroke_width":{"p":{"a":0,"k":30}}})";
        auto id16 = animation->gen(strokeWidthSlot);
        REQUIRE(id16 > 0);
        REQUIRE(animation->apply(id16) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id16) == Result::Success);
        REQUIRE(animation->del(id16) == Result::Success);

        const char* strokeWidthAnimatedSlot = R"({"stroke_width":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"s":[1],"t":0},{"s":[20],"t":100}]}}})";
        auto id17 = animation->gen(strokeWidthAnimatedSlot);
        REQUIRE(id17 > 0);
        REQUIRE(animation->apply(id17) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->del(id17) == Result::Success);

        //Slot Test 11: Bezier
        const char* bezierSlot = R"({"bezier_path":{"p":{"a":0,"k":{"c":true,"i":[[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0]],"v":[[-50,-50],[50,-50],[0,50]]}}}})";
        auto id18 = animation->gen(bezierSlot);
        REQUIRE(id18 > 0);
        REQUIRE(animation->apply(id18) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id18) == Result::Success);
        REQUIRE(animation->del(id18) == Result::Success);

        const char* bezierAnimatedSlot = R"({"bezier_path":{"p":{"a":1,"k":[{"i":{"x":0.833,"y":0.833},"o":{"x":0.167,"y":0.167},"t":0,"s":[{"c":true,"i":[[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0]],"v":[[-50,-50],[50,-50],[0,50]]}]},{"t":100,"s":[{"c":true,"i":[[0,0],[0,0],[0,0]],"o":[[0,0],[0,0],[0,0]],"v":[[-80,-80],[80,-80],[0,80]]}]}]}}})";
        auto id19 = animation->gen(bezierAnimatedSlot);
        REQUIRE(id19 > 0);
        REQUIRE(animation->apply(id19) == Result::Success);
        REQUIRE(animation->apply(0) == Result::Success);
        REQUIRE(animation->apply(id19) == Result::Success);
        REQUIRE(animation->del(id19) == Result::Success);
    }
    REQUIRE(Initializer::term() == Result::Success);
}

TEST_CASE("Lottie Marker", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        //Set marker name before loaded
        REQUIRE(animation->segment("sectionC") == Result::InsufficientCondition);

        // Get marker info before loaded
        float markerBegin, markerEnd;
        REQUIRE(animation->marker(0, &markerBegin, &markerEnd) == nullptr);

        //Animation load
        REQUIRE(picture->load(TEST_DIR"/segment.lot") == Result::Success);

        //Set marker
        REQUIRE(animation->segment("sectionA") == Result::Success);

        //Set marker by invalid name
        REQUIRE(animation->segment("") == Result::InvalidArguments);

        //Get marker count
        REQUIRE(animation->markersCnt() == 3);

        // Get marker name and segment by index
        REQUIRE(!strcmp(animation->marker(0, &markerBegin, &markerEnd), "sectionA"));
        REQUIRE(markerBegin == 0.0f);
        REQUIRE(markerEnd == 22.0f);

        REQUIRE(!strcmp(animation->marker(1, &markerBegin, &markerEnd), "sectionB"));
        REQUIRE(markerBegin == 22.0f);
        REQUIRE(markerEnd == 33.0f);

        REQUIRE(!strcmp(animation->marker(2, &markerBegin, &markerEnd), "sectionC"));
        REQUIRE(markerBegin == 33.0f);
        REQUIRE(markerEnd == 63.0f);

        // Get marker with only begin
        REQUIRE(!strcmp(animation->marker(0, &markerBegin, nullptr), "sectionA"));
        REQUIRE(markerBegin == 0.0f);

        // Get marker with only end
        REQUIRE(!strcmp(animation->marker(0, nullptr, &markerEnd), "sectionA"));
        REQUIRE(markerEnd == 22.0f);

        // Get marker by invalid index
        REQUIRE(animation->marker(-1, &markerBegin, &markerEnd) == nullptr);

        REQUIRE(animation->segment(nullptr) == Result::Success);
    }
    REQUIRE(Initializer::term() == Result::Success);
}

TEST_CASE("Lottie Motion Path By Arc Length", "[tvgLottie]")
{
    //a dot moving along a curved spatial key with a linear ease: at each frame it sits at the
    //same fraction of the curve's length as of the key's time, wherever the curve heads
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);
        auto picture = animation->picture();
        REQUIRE(picture->load(TEST_DIR"/motion_path.json") == Result::Success);

        const uint32_t w = 1000, h = 1200;
        auto buffer = new uint32_t[w * h];
        auto canvas = unique_ptr<SwCanvas>(SwCanvas::gen());
        REQUIRE(canvas->target(buffer, w, w, h, ColorSpace::ARGB8888) == Result::Success);
        REQUIRE(canvas->add(picture) == Result::Success);

        //the points at about a third and three quarters of the length, worked out to 1e-4 px
        const float frames[] = {49.0f, 124.0f};
        const float want[][2] = {{202.7004f, 172.5172f}, {234.7630f, 728.0811f}};
        for (int i = 0; i < 2; ++i) {
            memset(buffer, 0, w * h * sizeof(uint32_t));
            REQUIRE(animation->frame(frames[i]) == Result::Success);
            REQUIRE(canvas->update() == Result::Success);
            REQUIRE(canvas->draw(true) == Result::Success);
            REQUIRE(canvas->sync() == Result::Success);
            //the dot's centre: its coverage's centroid, pixel centres at half pixels
            double sum = 0, sx = 0, sy = 0;
            for (uint32_t y = 0; y < h; ++y) {
                for (uint32_t x = 0; x < w; ++x) {
                    auto a = (buffer[y * w + x] >> 24) & 0xff;
                    sum += a;
                    sx += a * (x + 0.5);
                    sy += a * (y + 0.5);
                }
            }
            REQUIRE(sum > 0);
            REQUIRE(fabs(sx / sum - want[i][0]) < 0.1);
            REQUIRE(fabs(sy / sum - want[i][1]) < 0.1);
        }
        delete[] buffer;
    }
    REQUIRE(Initializer::term() == Result::Success);
}

#ifdef THORVG_TTF_LOADER_SUPPORT
//the horizontal centre of the coverage in the top and the bottom quarter of a text layer's "I",
//drawn in the font loaded as `name` with its style "Italic"
static void italicStem(const char* name, double& top, double& bottom)
{
    auto json = string(R"({"v":"5.7.4","fr":30,"ip":0,"op":10,"w":200,"h":200,"ddd":0,"assets":[],)"
        R"("fonts":{"list":[{"fName":"NAME","fFamily":"NAME","fStyle":"Italic","ascent":75}]},)"
        R"("layers":[{"ddd":0,"ind":1,"ty":5,"sr":1,"ks":{"o":{"a":0,"k":100},"r":{"a":0,"k":0},)"
        R"("p":{"a":0,"k":[100,170,0]},"a":{"a":0,"k":[0,0,0]},"s":{"a":0,"k":[100,100,100]}},"ao":0,)"
        R"("t":{"d":{"k":[{"s":{"s":160,"f":"NAME","t":"I","j":2,"tr":0,"lh":192,"ls":0,"fc":[0,0,0]},"t":0}]},)"
        R"("p":{},"m":{"g":1,"a":{"a":0,"k":[0,0]}},"a":[]},"ip":0,"op":10,"st":0,"bm":0}]})");
    for (auto at = json.find("NAME"); at != string::npos; at = json.find("NAME")) json.replace(at, 4, name);

    auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
    REQUIRE(animation);
    auto picture = animation->picture();
    REQUIRE(picture->load(json.c_str(), json.size(), "lottie", nullptr, true) == Result::Success);

    const uint32_t w = 200, h = 200;
    auto buffer = new uint32_t[w * h]();
    auto canvas = unique_ptr<SwCanvas>(SwCanvas::gen());
    REQUIRE(canvas->target(buffer, w, w, h, ColorSpace::ARGB8888) == Result::Success);
    REQUIRE(canvas->add(picture) == Result::Success);
    REQUIRE(canvas->update() == Result::Success);
    REQUIRE(canvas->draw(true) == Result::Success);
    REQUIRE(canvas->sync() == Result::Success);

    //the rows the stem covers, and the centre of each end's quarter of them
    uint32_t y0 = h, y1 = 0;
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            if (buffer[y * w + x] >> 24) {
                y0 = std::min(y0, y);
                y1 = std::max(y1, y);
            }
        }
    }
    REQUIRE(y1 > y0 + 40);
    auto centre = [&](uint32_t from, uint32_t to) {
        double sum = 0, sx = 0;
        for (auto y = from; y < to; ++y) {
            for (uint32_t x = 0; x < w; ++x) {
                auto a = (buffer[y * w + x] >> 24) & 0xff;
                sum += a;
                sx += a * (x + 0.5);
            }
        }
        REQUIRE(sum > 0);
        return sx / sum;
    };
    auto q = (y1 - y0) / 4;
    top = centre(y0, y0 + q);
    bottom = centre(y1 - q, y1);
    delete[] buffer;
}

TEST_CASE("Lottie Italic Style Of An Italic Face", "[tvgLottie]")
{
    //a font whose style is named "Italic" is sheared only when its face isn't italic already:
    //an upright face's I leans, the same face marked italic (head.macStyle) stands as drawn
    REQUIRE(Initializer::init() == Result::Success);
    {
        ifstream file(TEST_DIR"/PublicSans-Regular.ttf", ios::binary);
        REQUIRE(file.is_open());
        string upright((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        auto italic = upright;
        //the head table, from the table records after the 12-byte header; its macStyle at 44
        auto u16 = [&](size_t at) { return (uint8_t(italic[at]) << 8) | uint8_t(italic[at + 1]); };
        auto u32 = [&](size_t at) { return (uint32_t(u16(at)) << 16) | u16(at + 2); };
        size_t head = 0;
        for (size_t i = 0; i < u16(4); ++i) {
            if (italic.compare(12 + i * 16, 4, "head") == 0) head = u32(12 + i * 16 + 8);
        }
        REQUIRE(head > 0);
        italic[head + 45] = char(uint8_t(italic[head + 45]) | 0x02);

        REQUIRE(Text::load("Upright Face", upright.data(), upright.size(), "ttf", true) == Result::Success);
        REQUIRE(Text::load("Italic Face", italic.data(), italic.size(), "ttf", true) == Result::Success);

        double top, bottom;
        italicStem("Upright Face", top, bottom);
        REQUIRE(top - bottom > 10.0);
        italicStem("Italic Face", top, bottom);
        REQUIRE(fabs(top - bottom) < 0.5);
    }
    REQUIRE(Initializer::term() == Result::Success);
}
#endif

TEST_CASE("Lottie Tween", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        REQUIRE(animation->tweenTo(10.0f) == Result::InsufficientCondition);
        REQUIRE(animation->tween(0.5f) == Result::InsufficientCondition);

        REQUIRE(picture->load(TEST_DIR"/tween.lot") == Result::Success);

        REQUIRE(animation->tween(0.5f) == Result::InsufficientCondition);

        REQUIRE(animation->frame(5.0f) == Result::Success);
        REQUIRE(animation->tweenTo(20.0f) == Result::Success);
        REQUIRE(animation->tween(0.0f) == Result::Success);
        REQUIRE(animation->tween(0.5f) == Result::Success);
        REQUIRE(animation->tween(1.0f) == Result::Success);

        REQUIRE(animation->tweenTo(30.0f) == Result::Success);
        REQUIRE(animation->frame(10.0f) == Result::Success);
        REQUIRE(animation->tween(0.5f) == Result::InsufficientCondition);

        REQUIRE(animation->tweenTo(40.0f) == Result::Success);
        REQUIRE(animation->tween(0.0f, 10.0f, 0.5f) == Result::Success);
        REQUIRE(animation->tween(0.75f) == Result::InsufficientCondition);
    }
    REQUIRE(Initializer::term() == Result::Success);
}

TEST_CASE("Lottie Quality", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        REQUIRE(animation->quality(50) == Result::InsufficientCondition);

        REQUIRE(picture->load(TEST_DIR"/test.lot") == Result::Success);

        //Set quality with minimum value
        REQUIRE(animation->quality(0) == Result::Success);

        //Set quality with default value
        REQUIRE(animation->quality(50) == Result::Success);

        //Set quality with maximum value
        REQUIRE(animation->quality(100) == Result::Success);

        //Set quality with various values
        REQUIRE(animation->quality(25) == Result::Success);
        REQUIRE(animation->quality(75) == Result::Success);

        //Set quality with invalid value (> 100)
        REQUIRE(animation->quality(101) == Result::InvalidArguments);
    }
    REQUIRE(Initializer::term() == Result::Success);
}

TEST_CASE("Lottie Asset Resolver", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        auto resolver = [](Paint* p, const char* src, void* data) -> bool {
            if (p->type() == Type::Picture) {
                string resolvedPath = string(TEST_DIR) + "/image/test.png";
                auto ret = static_cast<Picture*>(p)->load(resolvedPath.c_str());
                return (ret == Result::Success);
            } else if (p->type() == Type::Text) {
                string fontPath = string(TEST_DIR) + "/font/Arial.ttf";
                if (Text::load(fontPath.c_str()) != Result::Success) return false;
                auto ret = static_cast<Text*>(p)->font("Arial");
                return (ret == Result::Success);
            }
            return false;
        };

        // Test unset resolver
        REQUIRE(picture->resolver(resolver, nullptr) == Result::Success);
        REQUIRE(picture->resolver(nullptr, nullptr) == Result::Success);

        //Resolver Test (Image and Font)
        REQUIRE(picture->resolver(resolver, nullptr) == Result::Success);
        REQUIRE(picture->load(TEST_DIR"/resolver.json") == Result::Success);
        REQUIRE(animation->frame(animation->totalFrame() * 0.5f) == Result::Success);

        //Test that setting/unsetting resolver after load
        REQUIRE(picture->resolver(resolver, nullptr) == Result::InsufficientCondition);
        REQUIRE(picture->resolver(nullptr, nullptr) == Result::InsufficientCondition);
    }
    REQUIRE(Initializer::term() == Result::Success);
}


TEST_CASE("Lottie Audio Layer", "[tvgLottie]")
{
    REQUIRE(Initializer::init() == Result::Success);
    {
        auto animation = unique_ptr<LottieAnimation>(LottieAnimation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();

        int callCount = 0;
        LottieAudioResolver received{};

        auto resolver = [&](const LottieAudioResolver& info, void*) {
            ++callCount;
            received = info;
        };

        //Negative: register resolver before loaded
        REQUIRE(animation->resolver(resolver, nullptr) == Result::InsufficientCondition);

        REQUIRE(picture->load(TEST_DIR"/audio_layer.json") == Result::Success);

        //Frame updates without a registered resolver
        REQUIRE(animation->frame(15) == Result::Success);

        //Test Audio Resolver
        REQUIRE(animation->resolver(resolver, nullptr) == Result::Success);
        REQUIRE(animation->frame(1) == Result::Success);
        REQUIRE(callCount == 1);
        REQUIRE(received.active == true);
        REQUIRE(received.offset >= 0.0f);
        REQUIRE(received.volume == Approx(100.0f).margin(0.1f));
        REQUIRE(received.embedded == false);
        REQUIRE(received.src != nullptr);
        REQUIRE(received.size == 0);
        REQUIRE(received.mimeType == nullptr);

        //Resolver should not be invoked (no state change)
        callCount = 0;
        REQUIRE(animation->frame(5) == Result::Success);
        REQUIRE(callCount == 0);

        //Test Audio Resolver firing
        REQUIRE(animation->frame(20) == Result::Success);
        REQUIRE(callCount == 1);
        REQUIRE(received.active == false);
        REQUIRE(received.src != nullptr);   //source must remain identifiable on deactivation

        //Test Audio Resolver seeks again
        REQUIRE(animation->frame(6) == Result::Success);
        REQUIRE(received.active == true);
        REQUIRE(received.offset == Approx(6.0f / 60.0f).margin(0.01f));

        //Test Audio Resolver unregistration
        REQUIRE(animation->resolver(nullptr, nullptr) == Result::Success);
        callCount = 0;
        REQUIRE(animation->frame(25) == Result::Success);
        REQUIRE(callCount == 0);
    }
    REQUIRE(Initializer::term() == Result::Success);
}


TEST_CASE("Lottie Image on One Thread", "[tvgLottie]")
{
    //The Lottie loader builds on the only worker thread, and the image shown twice is decoded
    //there for its duplicate: it must not wait for a task queued behind itself
    REQUIRE(Initializer::init(1) == Result::Success);
    {
        auto canvas = unique_ptr<SwCanvas>(SwCanvas::gen());
        REQUIRE(canvas);

        uint32_t buffer[100*100] = {};
        REQUIRE(canvas->target(buffer, 100, 100, 100, ColorSpace::ARGB8888) == Result::Success);

        auto animation = unique_ptr<Animation>(Animation::gen());
        REQUIRE(animation);

        auto picture = animation->picture();
        REQUIRE(picture->load(TEST_DIR"/image.lot") == Result::Success);
        REQUIRE(animation->frame(10) == Result::Success);

        REQUIRE(canvas->add(picture) == Result::Success);
        REQUIRE(canvas->draw() == Result::Success);
        REQUIRE(canvas->sync() == Result::Success);

        //Both layers show the image, and nothing is between them
        REQUIRE(buffer[25 * 100 + 25] == 0xffff0000);
        REQUIRE(buffer[75 * 100 + 75] == 0xffff0000);
        REQUIRE(buffer[50 * 100 + 50] == 0);
    }
    REQUIRE(Initializer::term() == Result::Success);
}

#endif
