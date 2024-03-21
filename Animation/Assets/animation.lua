grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)


-- ##############################################
-- The Scene
-- ##############################################
scene = gr.node('scene')

-- the floor
plane = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(10, 1, 10)
plane:rotate('x', 90)

imSize = 512

gr.render(scene,
    'animation.png', imSize, imSize,
    {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})