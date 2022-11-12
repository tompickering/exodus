#include "news.h"

#include "assetpaths.h"

News::News() : ModeBase("News") {
}

void News::enter() {
    ModeBase::enter();
    Player *player = exostate.get_active_player();

    //draw_manager.draw();
}

ExodusMode News::update(float delta) {
    if (draw_manager.clicked()) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
