grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
mat1 = gr.material({0.7, 1.0, 0.7}, { 0.5, 0.5, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.2, 0.2, 0.2}, 25)

floor_mat = gr.material({1, 1.0, 1}, { 0.8, 0.8, 0.8}, 25)
wall_mat = gr.material({1,1,1}, {0.0, 0.0, 0.0}, 0)
s_mat =  gr.material({1,1,1}, {0.3,0.3,0.3}, 50)


-- ##############################################
-- Instances
-- ##############################################

-- the floor
inst_floor = gr.node('floors')

floor_texture_filename = './textures/Tile/Tile_04-512x512.png'
--floor_texture_filename = './textures/test2.png'


floor = gr.textureplane('floor', floor_texture_filename, {0.0, 1.0, 0.0})
inst_floor:add_child(floor)
floor:translate(-0.5, -10, -0.5)
floor:scale(50, 1, 50)
floor:set_material(floor_mat)

floor2 = gr.textureplane('floor2', floor_texture_filename, {0.0, 1.0, 0.0})
inst_floor:add_child(floor2)
floor2:translate(-1, -10, -0.5)
floor2:scale(50, 1, 50)
floor2:set_material(floor_mat)


floor3 = gr.textureplane('floor3', floor_texture_filename, {0.0, 1.0, 0.0})
inst_floor:add_child(floor3)
floor3:translate(0, -10, -0.5)
floor3:scale(50, 1, 50)
floor3:set_material(floor_mat)


-- wall
inst_wall = gr.node('walls')

wall_texture_filename = './textures/Brick/Brick_20-512x512.png'
--wall_texture_filename = './textures/test2.png'

wallz = -15
wally = 0.16

wall = gr.textureplane('wall', wall_texture_filename, {0.0, 0.0, 1.0})
inst_wall:add_child(wall)
wall:translate(-0.5, wally, wallz)
wall:scale(50, 50, 1)
--wall:translate(-2, wally, wallz)
--wall:scale(10, 10, 1)
wall:set_material(wall_mat)


wall2 = gr.textureplane('wall2', wall_texture_filename, {0.0, 0.0, 1.0})
inst_wall:add_child(wall2)
wall2:translate(-1, wally, wallz)
wall2:scale(50, 50, 1)
wall2:set_material(wall_mat)


wall3 = gr.textureplane('wall3', wall_texture_filename, {0.0, 0.0, 1.0})
inst_wall:add_child(wall3)
wall3:translate(-0, wally, wallz)
wall3:scale(50, 50, 1)
wall3:set_material(wall_mat)

offsets = wally - 1/3

wall4 = gr.textureplane('wall4', wall_texture_filename, {0.0, 0.0, 1.0})
inst_wall:add_child(wall4)
wall4:translate(-0.5, offsets, wallz)
wall4:scale(50, 50, 1)
wall4:set_material(wall_mat)


wall5 = gr.textureplane('wall5', wall_texture_filename, {0.0, 0.0, 1.0})
inst_wall:add_child(wall5)
wall5:translate(-1, offsets, wallz)
wall5:scale(50, 50, 1)
wall5:set_material(wall_mat)


wall6 = gr.textureplane('wall6', wall_texture_filename, {0.0, 0.0, 1.0})
inst_wall:add_child(wall6)
wall6:translate(0, offsets, wallz)
wall6:scale(50, 50, 1)
wall6:set_material(wall_mat)


-- Sphere
inst_sphere = gr.node('sphere')
s = gr.texturesphere('s', './textures/Metal/Metal_02-512x512.png')
s:set_material(s_mat)
inst_sphere:add_child(s)

------------------------------------------------------------------------------------------

-- ##############################################
-- The Scene
-- ##############################################
scene = gr.node('scene')

inst_sphere:translate(0.5, 0, 0)
inst_sphere:scale(6,6,6)

scene:add_child(inst_sphere)
scene:add_child(inst_floor)
scene:add_child(inst_wall)

imSize = 512
white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.5, 0.0, 0.5}, {1, 0, 0})

do_animation = false

if do_animation then
    for i = 1, 24*5 do
        s:rotate('y', 1);
        gr.render(scene,
            'Animation/animation_' .. string.format("%04d", i) .. '.png', imSize, imSize,
            {0, 2, 50}, {0, 0, -1}, {0, 1, 0}, 45,
            {0.4, 0.4, 0.4}, {white_light})
    end
else 
    gr.render(scene,
    'animation.png', imSize, imSize,
    {0, 2, 50}, {0, 0, -1}, {0, 1, 0}, 45,
    {0.4, 0.4, 0.4}, {white_light})
end