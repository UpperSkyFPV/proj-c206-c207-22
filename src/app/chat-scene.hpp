#pragma once

#include "conn.hpp"
#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "file.hpp"
#include "key.hpp"
#include "result.hpp"
#include "vector2.hpp"
#include <memory>
#include <sys/types.h>

namespace uppr::app {

class ChatScene : public eng::Scene {
public:
    ChatScene(std::shared_ptr<db::Connection> db_) : db{db_} {}

    void update(eng::Engine &engine) override {}

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override {
        using namespace fmt;

        const auto percent = [&](double v) {
            return (v / engine.get_max_frame_time()) * 100;
        };

        screen.print(transform.move(0, 0), "{:05d}/{:05d}us ({:0.2f})",
                     engine.get_frame_time(), engine.get_max_frame_time(),
                     percent(engine.get_frame_time()));
    }

    void mount(eng::Engine &engine) override {}

    void unmount(eng::Engine &engine) override {}

    static std::shared_ptr<ChatScene> make(std::shared_ptr<db::Connection> db) {
        return std::make_shared<ChatScene>(db);
    }

private:
    eng::Engine::EventBus::Handle u_handle;

    std::shared_ptr<db::Connection> db;
};
} // namespace uppr::app
