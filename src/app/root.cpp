#include "add-user-to-chat-scene.hpp"
#include "chat-scene.hpp"
#include "chatview-scene.hpp"
#include "conn.hpp"
#include "create-chat-scene.hpp"
#include "create-user-scene.hpp"
#include "engine.hpp"
#include "except.hpp"
#include "file.hpp"
#include "modal-scene.hpp"
#include "perf-scene.hpp"
#include "result.hpp"
#include "root.hpp"
#include "scene.hpp"
#include "select-view.hpp"
#include "sidebar-scene.hpp"
#include "stack-scene.hpp"
#include "state.hpp"
#include "stmt.hpp"
#include <memory>

namespace uppr::app {

shared_ptr<eng::Scene> make_scene_tree(eng::Engine &engine,
                                       shared_ptr<AppState> state) {
    // We put our scenes in a stack to allow for multiple scenes on top of each
    // other.
    const auto stack = eng::StackScene::make();

    stack->add_scene(engine, SidebarScene::make(ChatScene::make(state), state));

    // The main show
    {
        const auto create_user_modal =
            eng::ModalScene::make(CreateUserScene::make(state));

        const auto create_chat_modal =
            eng::ModalScene::make(CreateChatScene::make(state));

        const auto add_usertochat_modal =
            eng::ModalScene::make(AddUserToChatScene::make(state));

        stack->add_scene(engine, SelectViewScene::make(state, create_user_modal,
                                                       create_chat_modal,
                                                       add_usertochat_modal));
    }

    // The performance scene to shows performance stats
    stack->add_scene(engine, PerfScene::make());

    return stack;
}

int start_app(shared_ptr<term::TermScreen> term_screen) {
    // Initialize the database connection
    const auto database_connection = uppr::except::wrap_fatal_exception([] {
        const auto source = file::read_file_text("res/tables-safe.sql");
        const auto conn = db::Connection::open_ptr("db");

        conn->execute_many(source, [](const db::PreparedStmt &stmt) {
            LOG_F(9, "Executed statement@{} for start", fmt::ptr(&stmt));
        });

        return conn;
    });

    // Initialize the shared app state
    const auto app_state = std::make_shared<AppState>(database_connection);

    // Create our engine with FPS, screen and root scene (which we will insert
    // later)
    eng::Engine engine{30, term_screen, nullptr};

    // Create our scene tree and add it to the engine
    const auto root_scene = make_scene_tree(engine, app_state);
    engine.switch_scene(root_scene);

    // Run until user quits
    engine.run();

    return 0;
}
} // namespace uppr::app
