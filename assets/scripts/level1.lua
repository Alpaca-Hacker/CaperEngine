Level = {
    assets = {
        [0] = {
            type = "texture",
            id = "tank-blue",
            file = "./assets/images/kenney/tank_blue.png"
        },
        {
            type = "texture",
            id = "tank-dark",
            file = "./assets/images/kenney/tank_dark.png"
        },
        {
            type = "texture",
            id = "tank-red",
            file = "./assets/images/kenney/tank_red.png"
        },
        {
            type = "texture",
            id = "tank-sand",
            file = "./assets/images/kenney/tank_sand.png"
        },
        {
            type = "texture",
            id = "tank-green",
            file = "./assets/images/kenney/tank_green.png"
        },
        {
            type = "texture",
            id = "tree-green-large",
            file = "./assets/images/kenney/treeGreen_large.png"
        },
        {
            type = "texture",
            id = "tree-green-small",
            file = "./assets/images/kenney/treeGreen_small.png"
        },
        {
            type = "texture",
            id = "tree-brown-large",
            file = "./assets/images/kenney/treeBrown_large.png"
        },
        {
            type = "texture",
            id = "tree-brown-small",
            file = "./assets/images/kenney/treeBrown_small.png"
        },
        {
            type = "texture",
            id = "tilemap-image",
            file = "./assets/tilemaps/tanks.png"
        },
        {
            type = "texture",
            id = "chopper-image",
            file = "./assets/images/chopper-spritesheet.png"
        },
        {
            type = "texture",
            id = "radar-image",
            file = "./assets/images/radar.png"
        },
        {
            type = "texture",
            id = "bullet-image",
            file = "./assets/images/bullet.png"
        },
        {
            type = "font",
            id = "charriot-font",
            file = "./assets/fonts/charriot.ttf",
            size = 14
        },
        {
            type = "font",
            id = "covertops",
            file = "./assets/fonts/covertops.ttf",
            size = 25
        }
    },

    -- Tile map (Maybe replaced by tiled)
    tilemap = {
        map_file = "./assets/tilemaps/tanks.csv",
        asset_id = "tilemap-image",
        num_rows = 20,
        num_cols = 25,
        tile_size = 64,
        tile_scale = 1.0
    },
    -- Entities
    entities = {
        [0] = {
            -- Player
            tag = "Player",
            components = {
                transform = {
                    position = {
                        x = 100,
                        y = 100
                    },
                    scale = {
                        x = 1.0,
                        y = 1.0
                    },
                    rotation = 0.0
                },
                rigidbody = {
                    velocity = {
                        x = 0.0,
                        y = 0.0
                    }
                },
                sprite = {
                    texture_asset_id = "chopper-image",
                    width = 32,
                    height = 32,
                    z_index = 10
                },
                animation = {
                    num_frames = 2,
                    speed_rate = 10, -- fps
                    is_loop = true
                },
                boxcollider = {
                    width = 32,
                    height = 32,
                    offset = {
                        x = 0,
                        y = 0
                    }
                },
                health = 100,
                projectile_emitter = {
                    projectile_speed = 300,
                    repeat_freq = 300,
                    projectile_duration = 10000,
                    hit_damage = 10,
                    is_friendly = true
                },
                keyboard_controlled = {
                    up_velocity = {
                        x = 0,
                        y = -100
                    },
                    right_velocity = {
                        x = 100,
                        y = 0
                    },
                    down_velocity = {
                        x = 0,
                        y = 100
                    },
                    left_velocity = {
                        x = -100,
                        y = 0
                    }
                },
                camera_follow = true
            }

        },
        {
            -- tank 1
            tag = "Enemy",
            components = {
                transform = {
                    position = {
                        x = 500,
                        y = 490
                    },
                    scale = {
                        x = 1.0,
                        y = 1.0
                    },
                    rotation = -90.0
                },
                rigidbody = {
                    velocity = {
                        x = 50.0,
                        y = 0.0
                    }
                },
                sprite = {
                    texture_asset_id = "tank-blue",
                    width = 42,
                    height = 46,
                    z_index = 2
                },
                boxcollider = {
                    width = 42,
                    height = 46
                },
                health = 100,
                projectile_emitter = {
                    projectile_speed = 100,
                    repeat_freq = 5000,
                    projectile_duration = 10000,
                    hit_damage = 25,
                    is_friendly = false
                }
            }
        },
        {
            -- tank 2
            tag = "Enemy",
            components = {
                transform = {
                    position = {
                        x = 105,
                        y = 543
                    },
                    scale = {
                        x = 1.0,
                        y = 1.0
                    },
                    rotation = 0.0
                },
                rigidbody = {
                    velocity = {
                        x = 0.0,
                        y = 0.0
                    }
                },
                sprite = {
                    texture_asset_id = "tank-dark",
                    width = 42,
                    height = 46,
                    z_index = 2
                },
                boxcollider = {
                    width = 42,
                    height = 46
                },
                health = 100,
                projectile_emitter = {
                    projectile_speed = 100,
                    repeat_freq = 2000,
                    projectile_duration = 6000,
                    hit_damage = 10,
                    is_friendly = false
                }
            }
        },
        {
            -- Radar
            components = {
                transform = {
                    position = {
                        x = 1126, -- Window width - 74
                        y = 10
                    },
                    scale = {
                        x = 1.0,
                        y = 1.0
                    },
                    rotation = 0.0
                },
                rigidbody = {
                    velocity = {
                        x = 0.0,
                        y = 0.0
                    }
                },
                sprite = {
                    texture_asset_id = "radar-image",
                    width = 64,
                    height = 64,
                    z_index = 200,
                    is_fixed = true
                },
                animation = {
                    num_frames = 8,
                    speed_rate = 7, -- fps
                    is_loop = true
                }
            }
        },
        {
            -- tree 1
            tag = "Obstacle",
            components = {
                transform = {
                    position = {
                        x = 1000,
                        y = 482
                    },
                    scale = {
                        x = 1.0,
                        y = 1.0
                    },
                    rotation = 0.0
                },
                rigidbody = {
                    velocity = {
                        x = 0.0,
                        y = 0.0
                    }
                },
                sprite = {
                    texture_asset_id = "tree-brown-large",
                    width = 64,
                    height = 64,
                    z_index = 5
                },
                boxcollider = {
                    width = 64,
                    height = 64
                }
            }
        },
        {
            -- tree 2
            tag = "Obstacle",
            components = {
                transform = {
                    position = {
                        x = 300,
                        y = 482
                    },
                    scale = {
                        x = 1.0,
                        y = 1.0
                    },
                    rotation = 0.0
                },
                rigidbody = {
                    velocity = {
                        x = 0.0,
                        y = 0.0
                    }
                },
                sprite = {
                    texture_asset_id = "tree-green-large",
                    width = 64,
                    height = 64,
                    z_index = 5
                },
                boxcollider = {
                    width = 64,
                    height = 64
                }
            }
        },
        {
            -- Label
            components = {
                text = {
                    position = {
                        x = 575, -- width /2 - 25
                        y = 25
                    },
                    text = "Chopper",
                    font = "covertops",
                    colour = {
                        red = 100,
                        green = 149,
                        blue = 237,
                        alpha = 255
                    },
                    is_fixed = true
                }
            }
        }
    }
}
