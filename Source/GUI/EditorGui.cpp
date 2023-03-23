#include "EditorGui.h"

#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui_impl_glfw.h"
#include "../../Imgui/imgui_impl_vulkan.h"
#include "../../Imgui/imgui_internal.h"


 EditorGui::EditorGui(Scene *scene,VulkanBase *base)
     : m_scene(scene),
     m_base(base){
	//this->scene = scene;
}
 void EditorGui::createdescriptor(VulkanTexture &tex) {
     ds.push_back(ImGui_ImplVulkan_AddTexture(tex.imageSampler,tex.imageView,tex.imageLayout));

 }
 void EditorGui::run() {
	 setupStyle();
     setupGui();

     frameID += 1;
 }

 void EditorGui::tree(Node& node) {
     for (int i = 0; i < node.children.size(); i++) {
         auto c = IM_COL32(0, 255, 0, 255);
         if (i & 1) {
             c = IM_COL32(255, 255, 0, 255);
         }
         else {
             c = IM_COL32(0, 255, 0, 255);
         }

         ImGui::PushStyleColor(ImGuiCol_FrameBg, c);
         if (node.children[i].children.size()) {
             if (ImGui::TreeNode(m_scene->objects[node.children[i].ObjectID].name.c_str())) {

                 ImGui::TreePop();
                 ImGui::Indent();

                 tree(node.children[i]);
                 ImGui::Unindent();

             }
         }
         else {
             ImGui::Indent();
             ImGui::Selectable(m_scene->objects[node.children[i].ObjectID].name.c_str(), false);
             ImGui::Unindent();
         }

         ImGui::PopStyleColor();
     }
 }
 void EditorGui::setupGui() {
     /*
    if(frameID<3){
   m_scene->viewport_resolution.x = 512;
   m_scene->viewport_resolution.y = 512;
   vkDeviceWaitIdle(m_base->device);
   m_scene->recreate();
   m_scene->renderPass();
   }
   ImGui::Begin("Demo window");
   ImGui::Button("Hello!");
   ImGui::End();


   */
     ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar);

     ImGui::SetWindowSize(ImVec2(m_base->swapChain->swapChainExtent.width, m_base->swapChain->swapChainExtent.height));
     ImGui::SetWindowPos(ImVec2(0, 0));

     ImGuiIO& io = ImGui::GetIO();
     ImGuiID dockspace_id = ImGui::GetID("DockSpace");
     ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

     if (frameID == 1) {
         io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

         ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
         ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
         ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(m_base->swapChain->swapChainExtent.width, m_base->swapChain->swapChainExtent.height));

         auto dock_id_up = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.2f, nullptr, &dockspace_id);
         auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);

         auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

         auto dock_id_center = ImGui::DockBuilderGetCentralNode(dockspace_id);

         auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
         auto dock_id_right2 = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);


         ImGui::DockBuilderDockWindow("tools", dock_id_down);
         ImGui::DockBuilderDockWindow("scene", dock_id_left);
         ImGui::DockBuilderDockWindow("scene2", dock_id_right);
         ImGui::DockBuilderDockWindow("scene3", dock_id_right2);
         ImGui::DockBuilderDockWindow("Demo window", dock_id_up);
         ImGui::DockBuilderDockWindow("Viewport", dock_id_center->ID);
         ImGui::DockBuilderFinish(dockspace_id);
     }
    
     ImGui::End();
     //  ImGui::Begin("damn",0, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoBringToFrontOnFocus);
      // ImGui::SetWindowSize(ImVec2(base->swapChain->swapChainExtent.width, base->swapChain->swapChainExtent.height));
      // ImGui::SetWindowSize(ImVec2(100,100));
      // ImGui::SetWindowPos(ImVec2(0, 0));
      // ImGui::End();
    // ImGui::dockbu

     ImGuiID id = ImGui::GetID("Viewport");

     ImGui::SetWindowCollapsed(1);
     bool k1 = 1;
     ImGui::Begin("Viewport", &k1, ImGuiWindowFlags_NoTitleBar);
     ImVec2 viewport_size = ImGui::GetWindowSize();

     if (viewport_size.x != m_scene->viewport_resolution.x || viewport_size.y != m_scene->viewport_resolution.y) {
         m_scene->viewport_resolution.x = viewport_size.x;
         m_scene->viewport_resolution.y = viewport_size.y;
         vkDeviceWaitIdle(m_base->device);
         m_scene->recreate();
         m_scene->renderPass();
         ds[0] = ImGui_ImplVulkan_AddTexture(m_scene->quads[0]->textures[0].imageSampler, m_scene->framebuffers[1]->MultisampledColorImage->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
         //  vkDeviceWaitIdle(base->device);
           //cleanCommandBuffers();
         //  scene->framebuffers[0]->clean();
           //scene->framebuffers[0]->width = k.x;
           //scene->framebuffers[0]->height = k.y;
         //  scene->framebuffers[0]->recreate();


         //  scene->quads[0]->textures[0].imageView = scene->framebuffers[0]->MultisampledColorImage->imageView;
           //createCommandBuffers();

           //vkDeviceWaitIdle(base->device);
           //vkDeviceWaitIdle(base->device);
     }
     auto viewport_node = ImGui::DockBuilderGetCentralNode(dockspace_id);
     if (frameID == 2) {
         viewport_node->WantHiddenTabBarToggle = true;
     }
     ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
     //ImGui_ImplVulkan_AddTexture
  //   ImGui::Image(m_scene->quads[0]->descriptorSet, ImVec2{ viewportPanelSize.x, viewportPanelSize.y });
     ImGui::Image(ds[0], ImVec2{ viewportPanelSize.x, viewportPanelSize.y });
     ImGui::End();

     ImGui::Begin("tools");
     ImGui::End();

     ImGui::Begin("scene");
     tree(m_scene->rootNode);



     //  if (ImGui::TreeNode("node")) {
       //    if (ImGui::TreeNode("node1")) {
         //      ImGui::TreePop();
        //   }
       //    ImGui::TreePop();
     //  }

     ImGui::End();

     ImGui::Begin("scene2");
     ImGui::End();
     ImGui::Begin("scene3");
     ImGui::End();
     ImGui::Begin("Demo window");
     ImGui::BeginTabBar("List", 1);
     if (ImGui::BeginTabItem("Mesh List")) {
         for (int i = 0; i < m_scene->meshes.size(); i++) {
             ImGui::Selectable(m_scene->meshes[i].name.c_str(), false);
             //  ImGui::Text(scene->meshes[i].name.c_str());
         }
     }
     ImGui::EndTabItem();
     ImGui::EndTabBar();
     ImGui::End();

     ImGui::Render();
 }


 void EditorGui::setupStyle() {
     ImGuiStyle* style = &ImGui::GetStyle();
     ImVec4* colors = style->Colors;
     ImFont* font = ImGui::GetFont();
     font->FontSize = 19.;


     colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
     colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
     colors[ImGuiCol_WindowBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
     // colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
     colors[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
     colors[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
     colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
     colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
     colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
     colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
     colors[ImGuiCol_TitleBg] = ImVec4(0.2, 0.2, 0.2, 1.);
     colors[ImGuiCol_TitleBgActive] = ImVec4(0.4, 0.4, 0.4, 1.);
     colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.4, 0.4, 0.4, 1.);
     colors[ImGuiCol_MenuBarBg] = ImVec4(0.995f, 0.195f, 0.195f, 1.000f);
     colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
     colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
     colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
     colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
     colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
     colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
     colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
     colors[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
     colors[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
     colors[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
     colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
     colors[ImGuiCol_HeaderHovered] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
     colors[ImGuiCol_HeaderActive] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
     colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
     colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
     colors[ImGuiCol_SeparatorActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.000f);
     colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
     colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
     colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5f, 0.5f, 0.000f, 1.000f);
     colors[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
     colors[ImGuiCol_TabHovered] = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
     colors[ImGuiCol_TabActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
     colors[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
     colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
     colors[ImGuiCol_DockingPreview] = ImVec4(0.5f, 0.5f, 0.500f, 0.781f);
     colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
     colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
     colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5f, 0.5f, 0.500f, 1.000f);
     colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
     colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.5f, 0.5f, 0.000f, 1.000f);
     colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
     colors[ImGuiCol_DragDropTarget] = ImVec4(0.5f, 0.5f, 0.5f, 1.000f);
     colors[ImGuiCol_NavHighlight] = ImVec4(0.5f, 0.5f, 0.500f, 1.000f);
     colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.5f, 0.5f, 0.500f, 1.000f);
     colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
     colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

     // style->ChildRounding = 4.0f;
     style->FrameBorderSize = 1.0f;
     // style->FrameRounding = 2.0f;
     style->GrabMinSize = 7.0f;
     // style->PopupRounding = 2.0f;
     // style->ScrollbarRounding = 12.0f;
     style->ScrollbarSize = 13.0f;
     style->TabBorderSize = 1.0f;
     // style->TabRounding = 0.0f;
     // style->WindowRounding = 4.0f;
     style->WindowTitleAlign = ImVec2(0.5, 0.5);

     style->FramePadding = ImVec2(8, 6);
     //  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      // ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
      //ImGui::DockBuilderAddNode()


 }