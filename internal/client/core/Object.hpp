#pragma once

#include <core/Mesh.hpp>
#include <core/defines.hpp>

namespace core
{

    class RunScript {
        public:
        RunScript(std::function<int()> script) : script(script) {

        }

        int run() {
            int ret;
            try
            {
                ret = script();
                return ret;                
            }
            catch (const std::bad_function_call& e) {
                std::cout << e.what() << '\n';
                return -1;
            }
        };
        private:
        std::function<int()> script;
    };

    class Object
    {
public:
        Object() {
            renderable = false;
        }

        void set_mesh(MeshHandle handle) {
            set_renderable();
            mesh_handle = handle;
        }

        void set_program(ShaderProgramHandle handle) {
            program_handle = handle;
        }

        ShaderProgramHandle get_program_handle() {
            return program_handle;
        }

        MeshHandle get_mesh_handle() {
            return mesh_handle;
        }

        void run_frame_scripts() { // TODO: Lock guard scripts so they can't be appended and read from at the same time
            for (auto script : frame_scripts) {
                script.run();
            }
        }

        void run_init_scripts() { // TODO: Lock guard scripts so they can't be appended and read from at the same time
            for (auto script : init_scripts) {
                script.run();
            }
        }

        void set_renderable() {
            renderable = true;
        }

        void unset_renderable() {
            renderable = false;
        }

        bool is_renderable() {
            return renderable;
        }

        void push_frame_script(RunScript script) {
            frame_scripts.push_back(script);
        }

        void push_init_script(RunScript script) {
            init_scripts.push_back(script);
        }

private:
    MeshHandle mesh_handle;
    ShaderProgramHandle program_handle;
    std::vector<RunScript> frame_scripts;
    std::vector<RunScript> init_scripts;
    bool renderable;

    };
};