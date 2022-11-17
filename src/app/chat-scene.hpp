#pragma once

#include "chatinfo-scene.hpp"
#include "conn.hpp"
#include "eng/engine.hpp"
#include "eng/scene.hpp"
#include "file.hpp"
#include "key.hpp"
#include "result.hpp"
#include "state.hpp"
#include "vector2.hpp"
#include "writemsg-scene.hpp"
#include <memory>

namespace uppr::app {

class ChatScene : public eng::Scene {
public:
    ChatScene(shared_ptr<AppState> s)
        : state{s}, chat_info{std::make_unique<ChatInfoScene>(s)},
         write_msg{std::make_unique<WriteMsgScene>(s)} {}

    void update(eng::Engine &engine) override;

    void draw(eng::Engine &engine, term::Transform transform, term::Size size,
              term::TermScreen &screen) override;

    void mount(eng::Engine &engine) override;

    void unmount(eng::Engine &engine) override;

    static std::shared_ptr<ChatScene> make(shared_ptr<AppState> s) {
        return std::make_shared<ChatScene>(s);
    }

private:
    shared_ptr<AppState> state;
    unique_ptr<ChatInfoScene> chat_info;
    unique_ptr<WriteMsgScene> write_msg;
};
} // namespace uppr::app
