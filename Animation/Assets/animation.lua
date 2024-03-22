grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 10)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 5)


-- ##############################################
-- The Scene
-- ##############################################
scene = gr.node('scene')

-- the floor
plane1 = gr.textureplane('plane1', './textures/Tile/Tile_04-512x512.png', {0.0, 1.0, 0.0})
scene:add_child(plane1)
plane1:translate(-0.5, -10, -0.5)
plane1:scale(50, 1, 50)
plane1:set_material(mat1)

s = gr.sphere('s')
scene:add_child(s)
s:set_material(mat4)
s:scale(5,5,5)

imSize = 512

gr.render(scene,
    'animation.png', imSize, imSize,
    {0, 2, 50}, {0, 0, -1}, {0, 1, 0}, 45,
    {0.4, 0.4, 0.4}, {gr.light({100, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})