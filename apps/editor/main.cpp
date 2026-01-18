#include <Editor.hpp>
int main(int argc, char* argv[]) {
    auto editor = editor::Editor(argv[1], false);
    editor.start();

    return 0;
}
