-- A fractal created by spheres with

blue = gr.material({0.2, 0.2, 0.7}, {0.5, 0.5, 0.7}, 5)
orange = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

scene = gr.node('scene')
scene:rotate('X', 23)

arcblue = gr.node('arc')
s1 = gr.nh_sphere('sphere', {0, 0, 0}, 3.5)
arcblue:add_child(s1)
s1:set_material(blue)

arcorange = gr.node('arc')
s2 = gr.nh_sphere('sphere', {0, 0, 0}, 3.5)
arcorange:add_child(s2)
s2:set_material(orange)



i = 0
local function CreateChild(parent, x, y, z, depth)
    i = i + 1
    if depth <= 1 then
        return
    end

    local an_arc = gr.node('arc' .. (i))
    an_arc:translate(x, y, z)
    an_arc:scale(0.5, 0.5, 0.5)
    parent:add_child(an_arc)
    if depth % 2 == 0 then 
        an_arc:add_child(arcorange)
    else 
        an_arc:add_child(arcblue)
    end
    CreateChild(an_arc, 10, 0, 0, depth - 1)
    CreateChild(an_arc, 0, 10, 0, depth - 1)
    CreateChild(an_arc, 0, -10, 0, depth - 1)
    CreateChild(an_arc, -10, 0, 0, depth - 1)
    CreateChild(an_arc, 0, 0, 10, depth - 1)
    CreateChild(an_arc, 0, 0, -10, depth - 1)
    
end

arc1 = gr.node('arc1')
scene:add_child(arc1)
arc1:add_child(arcblue)
arc1:rotate('x', 45)
arc1:rotate('y', 45)
-- arc1:rotate('z', 45)
local depth = 4

CreateChild(arc1, 10, 0, 0, depth)
CreateChild(arc1, 0, 10, 0, depth)
CreateChild(arc1, -10, 0, 0, depth)
CreateChild(arc1, 0, -10, 0, depth)
CreateChild(arc1, 0, 0, 10, depth)
CreateChild(arc1, 0, 0, -10, depth)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})


steldodec = gr.mesh( 'dodec', 'Assets/smstdodeca.obj' )
steldodec:set_material(mat3)
steldodec:translate(-100, -300, -1000)
scene:add_child(steldodec)

-- the floor

plane = gr.mesh('plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(50, 30, 50)
plane:translate(0, -30, 0)

imSize = 512

gr.render(scene,
	  'sample.png', imSize, imSize,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light, magenta_light})
