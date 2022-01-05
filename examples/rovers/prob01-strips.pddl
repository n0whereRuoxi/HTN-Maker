( define ( problem probname )
  ( :domain Rover )
  ( :requirements :strips :typing :equality )

  ( :objects
    general - lander
    wp000 - waypoint
    wp001 - waypoint
    wp002 - waypoint
    wp003 - waypoint
    wp004 - waypoint
    rover000 - rover
    rover001 - rover
    store000 - store
    store001 - store
    mode000 - mode
    mode001 - mode
    obj000 - objective
    obj001 - objective
    obj002 - objective
    cam000 - camera
    cam001 - camera
  )

  ( :init
    ( visible wp000 wp001 )
    ( visible wp000 wp002 )
    ( visible wp000 wp003 )
    ( visible wp000 wp004 )
    ( visible wp001 wp000 )
    ( visible wp001 wp002 )
    ( visible wp001 wp004 )
    ( visible wp002 wp000 )
    ( visible wp002 wp001 )
    ( visible wp002 wp003 )
    ( visible wp003 wp000 )
    ( visible wp003 wp002 )
    ( visible wp003 wp004 )
    ( visible wp004 wp000 )
    ( visible wp004 wp001 )
    ( visible wp004 wp003 )
    ( at_soil_sample wp002 )
    ( at_rock_sample wp002 )
    ( at_rock_sample wp003 )
    ( at_lander general wp003 )
    ( channel_free general )
    ( at rover000 wp002 )
    ( at rover001 wp000 )
    ( available rover000 )
    ( available rover001 )
    ( equipped_for_soil_analysis rover000 )
    ( equipped_for_soil_analysis rover001 )
    ( equipped_for_rock_analysis rover001 )
    ( equipped_for_imaging rover000 )
    ( can_traverse rover000 wp000 wp001 )
    ( can_traverse rover000 wp000 wp003 )
    ( can_traverse rover000 wp001 wp004 )
    ( can_traverse rover000 wp002 wp000 )
    ( can_traverse rover000 wp002 wp001 )
    ( can_traverse rover000 wp003 wp000 )
    ( can_traverse rover000 wp003 wp002 )
    ( can_traverse rover000 wp003 wp004 )
    ( can_traverse rover000 wp004 wp000 )
    ( can_traverse rover000 wp004 wp003 )
    ( can_traverse rover001 wp000 wp001 )
    ( can_traverse rover001 wp000 wp003 )
    ( can_traverse rover001 wp000 wp004 )
    ( can_traverse rover001 wp001 wp004 )
    ( can_traverse rover001 wp002 wp001 )
    ( can_traverse rover001 wp002 wp003 )
    ( can_traverse rover001 wp003 wp000 )
    ( can_traverse rover001 wp003 wp002 )
    ( can_traverse rover001 wp004 wp000 )
    ( can_traverse rover001 wp004 wp003 )
    ( store_of store000 rover000 )
    ( store_of store001 rover001 )
    ( empty store000 )
    ( empty store001 )
    ( visible_from obj000 wp003 )
    ( visible_from obj001 wp004 )
    ( visible_from obj002 wp000 )
    ( visible_from obj002 wp001 )
    ( visible_from obj002 wp004 )
    ( on_board cam000 rover000 )
    ( on_board cam001 rover000 )
    ( supports cam000 mode000 )
    ( supports cam000 mode001 )
    ( supports cam001 mode000 )
    ( supports cam001 mode001 )
    ( calibration_target cam000 obj001 )
    ( calibration_target cam001 obj001 )
  )
  ( :goal
    ( and
      ( communicated_soil_data wp002 )
      ( communicated_rock_data wp002 )
      ( communicated_rock_data wp003 )
      ( communicated_image_data obj001 mode000 )
      ( communicated_image_data obj001 mode001 )
      ( communicated_image_data obj002 mode000 )
    )
  )
)

