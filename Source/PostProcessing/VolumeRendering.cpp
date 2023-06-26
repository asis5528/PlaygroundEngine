#include "VolumeRendering.h"
#include "../Graphics/VulkanUtils.h"


VolumeRendering::VolumeRendering(VulkanBase* base, Framebuffer* previousFB)
{
	this->base = base;
    this->previousFB = previousFB;
    Framebuffer* frameBuffer_quad = new Framebuffer(base->vulkandevice, base->swapChain->swapChainExtent.width, base->swapChain->swapChainExtent.height, base->vulkandevice->getMaxUsableSampleCount(), base->swapChain->swapChainImageFormat, utils::findDepthFormat(base->vulkandevice->physicalDevice));
    framebuffers.push_back(frameBuffer_quad);
    Quad* quad = new Quad(base, framebuffers[0]);
    quad->createGraphicsPipeline("shaders/quadvert.spv", "shaders/quadfrag.spv");
    VulkanTexture quadtex;
    quadtex.imageView = previousFB->MultisampledColorImage->imageView;
    quadtex.imageSampler = base->vbuffer->createTextureSampler(1);
    quadtex.imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    quad->textures.push_back(quadtex);
    quad->createdescriptors();
    quads.push_back(quad);


}

void VolumeRendering::renderPass(VkCommandBuffer commandBuffer)
{
    framebuffers[0]->beginRenderPass(commandBuffer);
    quads[0]->draw(commandBuffer);
    vkCmdEndRenderPass(commandBuffer);
}

void VolumeRendering::recreate(VkExtent2D ex)
{
    for (Framebuffer*& framebuffer : framebuffers) {

        framebuffer->clean();
        framebuffer->width = ex.width;
        framebuffer->height = ex.height;
        framebuffer->recreate();
    }
    for (Quad*& quad : quads) {
        quad->textures[0].imageView = previousFB->MultisampledColorImage->imageView;
        quad->graphicsPipeline->clean();
        quad->recreateGraphicsPipeline(ex);
        quad->cleandescriptors();
        quad->createdescriptors();
    }
}

VolumeRendering::~VolumeRendering()
{
    for (Quad*& quad : quads) {
        delete quad->graphicsPipeline;
        delete quad;
    }
    for (Framebuffer*& fb : framebuffers) {
        delete fb;
    }
}
