-- frontend-menu.lua

-- Note: we don't need to load frontend-menu.dat from here as it is
-- loaded by the C code when necessary.


local faces_table = {
    "/frontend/menu/faces/faces000",
    "/frontend/menu/faces/faces001",
    "/frontend/menu/faces/faces002",
    "/frontend/menu/faces/faces003",
    "/frontend/menu/faces/faces004",
    "/frontend/menu/faces/faces005",
    "/frontend/menu/faces/faces006",
    "/frontend/menu/faces/faces007",
    "/frontend/menu/faces/faces008",
    "/frontend/menu/faces/faces009",
    "/frontend/menu/faces/faces010",
    "/frontend/menu/faces/faces011",
    "/frontend/menu/faces/faces012",
    "/frontend/menu/faces/faces013",
    "/frontend/menu/faces/faces014",
    "/frontend/menu/faces/faces015",
    "/frontend/menu/faces/faces016",
    "/frontend/menu/faces/faces017",
    "/frontend/menu/faces/faces018",
    "/frontend/menu/faces/faces019",
    "/frontend/menu/faces/faces021",
    "/frontend/menu/faces/faces022",
    "/frontend/menu/faces/faces023",
    "/frontend/menu/faces/faces024",
    "/frontend/menu/faces/faces025",
    "/frontend/menu/faces/faces026",
    "/frontend/menu/faces/faces027"
}

function _internal_get_random_face ()
    local i = math.random (table.getn (faces_table))
    return faces_table [i]
end
