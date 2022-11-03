#include "conn.hpp"
#include "create-user-scene.hpp"
#include "engine.hpp"
#include "except.hpp"
#include "file.hpp"
#include "modal-scene.hpp"
#include "perf-scene.hpp"
#include "result.hpp"
#include "root.hpp"
#include "scene.hpp"
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

    // The performance scene to shows performance stats
    stack->add_scene(engine, PerfScene::make());

    // The main show
    {
        const auto modal = eng::ModalScene::make(CreateUserScene::make(state),
                                                 PerfScene::make(), {100, 20});

        modal->show_modal(engine);

        stack->add_scene(engine, modal);
    }

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