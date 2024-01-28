
cd ../Resources/Shaders

glslc single_mesh.vert -o single_mesh_vert.spv
glslc batched_meshes.vert -o batched_meshes_vert.spv
glslc shadow.vert -o shadow_vert.spv
glslc fullscreen_quad.vert -o fullscreen_quad_vert.spv

glslc single_material.frag -o single_material_frag.spv
glslc single_material_pbr.frag -o single_material_pbr_frag.spv
glslc single_material_pbr_transparent.frag -o single_material_pbr_transparent_frag.spv
glslc fullscreen_texture.frag -o fullscreen_texture_frag.spv

glslc depth_reduce.comp -o depth_reduce_comp.spv
glslc view_culling.comp -o view_culling_comp.spv
glslc shadow_culling.comp -o shadow_culling_comp.spv

glslc debug_view_culling.comp -o debug_view_culling_comp.spv
