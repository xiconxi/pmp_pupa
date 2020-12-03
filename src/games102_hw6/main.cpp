//
// Created by pupa on 12/2/20.
//

#include <pupa/vis/MeshViewer.h>
#include <pupa/pmp/MinimalAreaSurface.h>

namespace pupa_vis {

struct MeshViewerData
{
    explicit MeshViewerData(pmp::SurfaceMesh& mesh) : minimal_surf_(mesh) {}

    pmp_pupa::MinimalAreaSurface minimal_surf_;

    int explicit_iterations_{100};
    bool explicit_running_{false};

    int implicit_iterations_{2};
    bool implicit_running_{false};
    bool boundary_smoothing_{false};

    float lambda_{0.2};
};

static char* files[]{"Balls.obj", "Bunny_head.obj", "Cat_head.obj", "David328.obj", "Nefertiti_face.obj"};

void MeshViewer::process_imgui()
{
    if (ImGui::CollapsingHeader("Minimal Area Surface"))
    {
#if __EMSCRIPTEN__
        static int n = 4;
        ImGui::Combo("Files", &n, files, 5);
        ImGui::SameLine();
#endif
        if (ImGui::Button("Reload Mesh"))
        {
            data_ = nullptr;
#if __EMSCRIPTEN__
            load_mesh(files[n]);
#else
            load_mesh(filename_.data());
#endif

            data_ = std::make_shared<MeshViewerData>(mesh_);
        }

        ImGui::SliderFloat("lambda", &data_->lambda_, 0.01, 0.8);

        ImGui::SliderInt("Implicit iters", &data_->implicit_iterations_, 1, 10);
        if (ImGui::Button("Implicit Minimal Surf"))
            data_->implicit_running_ = !data_->implicit_running_;

        ImGui::SliderInt("Explicit iters", &data_->explicit_iterations_, 1, 200);
        if (ImGui::Button("Explicit Minimal Surf"))
            data_->explicit_running_ = !data_->explicit_running_;
        ImGui::SameLine();
        ImGui::Checkbox("Explicit Minimal Boundary", &data_->boundary_smoothing_);
    }
}

void MeshViewer::do_processing()
{
    if (data_ == nullptr)
        data_ = std::make_shared<MeshViewerData>(mesh_);

    if (data_->explicit_running_ && data_->explicit_iterations_)
    {
        if (data_->boundary_smoothing_)
            data_->minimal_surf_.boundary_explicit_iterate(data_->lambda_ * 0.5);
        data_->minimal_surf_.explicit_iterate(data_->lambda_);
        data_->explicit_iterations_--;
        update_mesh();
    }
    else if (data_->explicit_running_)
    {
        data_->explicit_running_ = false;
        data_->explicit_iterations_ = 100;
    }

    if (data_->implicit_running_ && data_->implicit_iterations_)
    {
        data_->minimal_surf_.implicit_iterate(data_->lambda_);
        data_->implicit_iterations_--;
        update_mesh();
    }
    else if (data_->implicit_running_)
    {
        data_->implicit_running_ = false;
        data_->implicit_iterations_ = 5;
    }
}

} // namespace pupa_vis

int main(int argc, char* argv[])
{
    pupa_vis::MeshViewer mesh_viewer("Games102-HW6", 800, 600);

    mesh_viewer.load_mesh(argc == 2 ? argv[1] : "Nefertiti_face.obj");
    mesh_viewer.run();
}