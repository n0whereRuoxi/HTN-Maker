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
    wp005 - waypoint
    wp006 - waypoint
    wp007 - waypoint
    wp008 - waypoint
    wp009 - waypoint
    wp010 - waypoint
    wp011 - waypoint
    wp012 - waypoint
    wp013 - waypoint
    wp014 - waypoint
    rover000 - rover
    rover001 - rover
    rover002 - rover
    store000 - store
    store001 - store
    store002 - store
    mode000 - mode
    mode001 - mode
    mode002 - mode
    mode003 - mode
    obj000 - objective
    obj001 - objective
    obj002 - objective
    obj003 - objective
    obj004 - objective
    obj005 - objective
    cam000 - camera
    cam001 - camera
    cam002 - camera
  )

  ( :init
    ( visible wp000 wp001 )
    ( visible wp000 wp002 )
    ( visible wp000 wp003 )
    ( visible wp000 wp004 )
    ( visible wp000 wp005 )
    ( visible wp000 wp006 )
    ( visible wp000 wp007 )
    ( visible wp000 wp008 )
    ( visible wp000 wp011 )
    ( visible wp000 wp013 )
    ( visible wp000 wp014 )
    ( visible wp001 wp000 )
    ( visible wp001 wp002 )
    ( visible wp001 wp003 )
    ( visible wp001 wp004 )
    ( visible wp001 wp005 )
    ( visible wp001 wp006 )
    ( visible wp001 wp007 )
    ( visible wp001 wp009 )
    ( visible wp001 wp011 )
    ( visible wp001 wp013 )
    ( visible wp001 wp014 )
    ( visible wp002 wp000 )
    ( visible wp002 wp001 )
    ( visible wp002 wp003 )
    ( visible wp002 wp006 )
    ( visible wp002 wp007 )
    ( visible wp002 wp008 )
    ( visible wp002 wp009 )
    ( visible wp002 wp010 )
    ( visible wp002 wp011 )
    ( visible wp002 wp013 )
    ( visible wp002 wp014 )
    ( visible wp003 wp000 )
    ( visible wp003 wp001 )
    ( visible wp003 wp002 )
    ( visible wp003 wp005 )
    ( visible wp003 wp006 )
    ( visible wp003 wp007 )
    ( visible wp003 wp008 )
    ( visible wp003 wp009 )
    ( visible wp003 wp011 )
    ( visible wp003 wp013 )
    ( visible wp003 wp014 )
    ( visible wp004 wp000 )
    ( visible wp004 wp001 )
    ( visible wp004 wp005 )
    ( visible wp004 wp006 )
    ( visible wp004 wp008 )
    ( visible wp004 wp010 )
    ( visible wp004 wp011 )
    ( visible wp004 wp012 )
    ( visible wp004 wp014 )
    ( visible wp005 wp000 )
    ( visible wp005 wp001 )
    ( visible wp005 wp003 )
    ( visible wp005 wp004 )
    ( visible wp005 wp007 )
    ( visible wp005 wp008 )
    ( visible wp005 wp012 )
    ( visible wp005 wp013 )
    ( visible wp005 wp014 )
    ( visible wp006 wp000 )
    ( visible wp006 wp001 )
    ( visible wp006 wp002 )
    ( visible wp006 wp003 )
    ( visible wp006 wp004 )
    ( visible wp006 wp007 )
    ( visible wp006 wp010 )
    ( visible wp006 wp011 )
    ( visible wp006 wp012 )
    ( visible wp006 wp014 )
    ( visible wp007 wp000 )
    ( visible wp007 wp001 )
    ( visible wp007 wp002 )
    ( visible wp007 wp003 )
    ( visible wp007 wp005 )
    ( visible wp007 wp006 )
    ( visible wp007 wp009 )
    ( visible wp007 wp010 )
    ( visible wp007 wp011 )
    ( visible wp008 wp000 )
    ( visible wp008 wp002 )
    ( visible wp008 wp003 )
    ( visible wp008 wp004 )
    ( visible wp008 wp005 )
    ( visible wp008 wp009 )
    ( visible wp008 wp010 )
    ( visible wp008 wp011 )
    ( visible wp008 wp012 )
    ( visible wp008 wp013 )
    ( visible wp009 wp001 )
    ( visible wp009 wp002 )
    ( visible wp009 wp003 )
    ( visible wp009 wp007 )
    ( visible wp009 wp008 )
    ( visible wp009 wp010 )
    ( visible wp009 wp011 )
    ( visible wp009 wp013 )
    ( visible wp010 wp002 )
    ( visible wp010 wp004 )
    ( visible wp010 wp006 )
    ( visible wp010 wp007 )
    ( visible wp010 wp008 )
    ( visible wp010 wp009 )
    ( visible wp010 wp011 )
    ( visible wp010 wp013 )
    ( visible wp011 wp000 )
    ( visible wp011 wp001 )
    ( visible wp011 wp002 )
    ( visible wp011 wp003 )
    ( visible wp011 wp004 )
    ( visible wp011 wp006 )
    ( visible wp011 wp007 )
    ( visible wp011 wp008 )
    ( visible wp011 wp009 )
    ( visible wp011 wp010 )
    ( visible wp011 wp012 )
    ( visible wp011 wp014 )
    ( visible wp012 wp004 )
    ( visible wp012 wp005 )
    ( visible wp012 wp006 )
    ( visible wp012 wp008 )
    ( visible wp012 wp011 )
    ( visible wp012 wp014 )
    ( visible wp013 wp000 )
    ( visible wp013 wp001 )
    ( visible wp013 wp002 )
    ( visible wp013 wp003 )
    ( visible wp013 wp005 )
    ( visible wp013 wp008 )
    ( visible wp013 wp009 )
    ( visible wp013 wp010 )
    ( visible wp014 wp000 )
    ( visible wp014 wp001 )
    ( visible wp014 wp002 )
    ( visible wp014 wp003 )
    ( visible wp014 wp004 )
    ( visible wp014 wp005 )
    ( visible wp014 wp006 )
    ( visible wp014 wp011 )
    ( visible wp014 wp012 )
    ( at_soil_sample wp003 )
    ( at_soil_sample wp004 )
    ( at_soil_sample wp009 )
    ( at_soil_sample wp010 )
    ( at_soil_sample wp012 )
    ( at_soil_sample wp014 )
    ( at_rock_sample wp004 )
    ( at_rock_sample wp008 )
    ( at_rock_sample wp009 )
    ( at_rock_sample wp010 )
    ( at_rock_sample wp011 )
    ( at_lander general wp001 )
    ( channel_free general )
    ( at rover000 wp004 )
    ( at rover001 wp013 )
    ( at rover002 wp002 )
    ( available rover000 )
    ( available rover001 )
    ( available rover002 )
    ( equipped_for_soil_analysis rover001 )
    ( equipped_for_soil_analysis rover002 )
    ( equipped_for_rock_analysis rover001 )
    ( equipped_for_rock_analysis rover002 )
    ( equipped_for_imaging rover000 )
    ( equipped_for_imaging rover002 )
    ( can_traverse rover000 wp000 wp002 )
    ( can_traverse rover000 wp000 wp005 )
    ( can_traverse rover000 wp000 wp006 )
    ( can_traverse rover000 wp000 wp008 )
    ( can_traverse rover000 wp000 wp011 )
    ( can_traverse rover000 wp000 wp013 )
    ( can_traverse rover000 wp001 wp004 )
    ( can_traverse rover000 wp001 wp005 )
    ( can_traverse rover000 wp001 wp014 )
    ( can_traverse rover000 wp002 wp001 )
    ( can_traverse rover000 wp002 wp009 )
    ( can_traverse rover000 wp002 wp013 )
    ( can_traverse rover000 wp002 wp014 )
    ( can_traverse rover000 wp003 wp001 )
    ( can_traverse rover000 wp003 wp002 )
    ( can_traverse rover000 wp003 wp005 )
    ( can_traverse rover000 wp003 wp006 )
    ( can_traverse rover000 wp003 wp008 )
    ( can_traverse rover000 wp004 wp001 )
    ( can_traverse rover000 wp004 wp005 )
    ( can_traverse rover000 wp004 wp010 )
    ( can_traverse rover000 wp005 wp003 )
    ( can_traverse rover000 wp005 wp004 )
    ( can_traverse rover000 wp005 wp008 )
    ( can_traverse rover000 wp005 wp012 )
    ( can_traverse rover000 wp006 wp001 )
    ( can_traverse rover000 wp006 wp003 )
    ( can_traverse rover000 wp006 wp010 )
    ( can_traverse rover000 wp006 wp012 )
    ( can_traverse rover000 wp007 wp001 )
    ( can_traverse rover000 wp007 wp002 )
    ( can_traverse rover000 wp007 wp003 )
    ( can_traverse rover000 wp007 wp005 )
    ( can_traverse rover000 wp007 wp009 )
    ( can_traverse rover000 wp007 wp010 )
    ( can_traverse rover000 wp007 wp011 )
    ( can_traverse rover000 wp008 wp000 )
    ( can_traverse rover000 wp008 wp003 )
    ( can_traverse rover000 wp008 wp010 )
    ( can_traverse rover000 wp008 wp011 )
    ( can_traverse rover000 wp008 wp012 )
    ( can_traverse rover000 wp009 wp001 )
    ( can_traverse rover000 wp009 wp002 )
    ( can_traverse rover000 wp009 wp008 )
    ( can_traverse rover000 wp009 wp010 )
    ( can_traverse rover000 wp009 wp013 )
    ( can_traverse rover000 wp010 wp002 )
    ( can_traverse rover000 wp010 wp009 )
    ( can_traverse rover000 wp010 wp011 )
    ( can_traverse rover000 wp011 wp000 )
    ( can_traverse rover000 wp011 wp001 )
    ( can_traverse rover000 wp011 wp007 )
    ( can_traverse rover000 wp011 wp008 )
    ( can_traverse rover000 wp011 wp010 )
    ( can_traverse rover000 wp011 wp012 )
    ( can_traverse rover000 wp012 wp011 )
    ( can_traverse rover000 wp013 wp000 )
    ( can_traverse rover000 wp013 wp001 )
    ( can_traverse rover000 wp013 wp002 )
    ( can_traverse rover000 wp013 wp005 )
    ( can_traverse rover000 wp013 wp008 )
    ( can_traverse rover000 wp013 wp009 )
    ( can_traverse rover000 wp014 wp001 )
    ( can_traverse rover000 wp014 wp005 )
    ( can_traverse rover001 wp000 wp003 )
    ( can_traverse rover001 wp000 wp004 )
    ( can_traverse rover001 wp000 wp005 )
    ( can_traverse rover001 wp000 wp007 )
    ( can_traverse rover001 wp000 wp011 )
    ( can_traverse rover001 wp001 wp000 )
    ( can_traverse rover001 wp001 wp004 )
    ( can_traverse rover001 wp001 wp006 )
    ( can_traverse rover001 wp001 wp007 )
    ( can_traverse rover001 wp001 wp009 )
    ( can_traverse rover001 wp001 wp011 )
    ( can_traverse rover001 wp001 wp013 )
    ( can_traverse rover001 wp001 wp014 )
    ( can_traverse rover001 wp002 wp001 )
    ( can_traverse rover001 wp002 wp010 )
    ( can_traverse rover001 wp002 wp011 )
    ( can_traverse rover001 wp002 wp014 )
    ( can_traverse rover001 wp003 wp000 )
    ( can_traverse rover001 wp003 wp002 )
    ( can_traverse rover001 wp003 wp005 )
    ( can_traverse rover001 wp003 wp006 )
    ( can_traverse rover001 wp003 wp007 )
    ( can_traverse rover001 wp003 wp011 )
    ( can_traverse rover001 wp003 wp013 )
    ( can_traverse rover001 wp004 wp000 )
    ( can_traverse rover001 wp004 wp006 )
    ( can_traverse rover001 wp004 wp008 )
    ( can_traverse rover001 wp004 wp010 )
    ( can_traverse rover001 wp004 wp011 )
    ( can_traverse rover001 wp004 wp014 )
    ( can_traverse rover001 wp005 wp000 )
    ( can_traverse rover001 wp005 wp004 )
    ( can_traverse rover001 wp005 wp007 )
    ( can_traverse rover001 wp005 wp012 )
    ( can_traverse rover001 wp005 wp013 )
    ( can_traverse rover001 wp005 wp014 )
    ( can_traverse rover001 wp006 wp001 )
    ( can_traverse rover001 wp006 wp010 )
    ( can_traverse rover001 wp006 wp011 )
    ( can_traverse rover001 wp006 wp012 )
    ( can_traverse rover001 wp006 wp014 )
    ( can_traverse rover001 wp007 wp000 )
    ( can_traverse rover001 wp007 wp003 )
    ( can_traverse rover001 wp007 wp009 )
    ( can_traverse rover001 wp007 wp010 )
    ( can_traverse rover001 wp008 wp009 )
    ( can_traverse rover001 wp008 wp013 )
    ( can_traverse rover001 wp009 wp001 )
    ( can_traverse rover001 wp009 wp002 )
    ( can_traverse rover001 wp009 wp003 )
    ( can_traverse rover001 wp009 wp007 )
    ( can_traverse rover001 wp009 wp008 )
    ( can_traverse rover001 wp009 wp011 )
    ( can_traverse rover001 wp010 wp004 )
    ( can_traverse rover001 wp010 wp006 )
    ( can_traverse rover001 wp010 wp009 )
    ( can_traverse rover001 wp010 wp011 )
    ( can_traverse rover001 wp010 wp013 )
    ( can_traverse rover001 wp011 wp000 )
    ( can_traverse rover001 wp011 wp002 )
    ( can_traverse rover001 wp011 wp004 )
    ( can_traverse rover001 wp011 wp006 )
    ( can_traverse rover001 wp011 wp009 )
    ( can_traverse rover001 wp011 wp012 )
    ( can_traverse rover001 wp012 wp004 )
    ( can_traverse rover001 wp012 wp005 )
    ( can_traverse rover001 wp012 wp006 )
    ( can_traverse rover001 wp012 wp008 )
    ( can_traverse rover001 wp012 wp011 )
    ( can_traverse rover001 wp012 wp014 )
    ( can_traverse rover001 wp013 wp001 )
    ( can_traverse rover001 wp013 wp008 )
    ( can_traverse rover001 wp013 wp010 )
    ( can_traverse rover001 wp014 wp000 )
    ( can_traverse rover001 wp014 wp001 )
    ( can_traverse rover001 wp014 wp004 )
    ( can_traverse rover001 wp014 wp005 )
    ( can_traverse rover001 wp014 wp012 )
    ( can_traverse rover002 wp000 wp001 )
    ( can_traverse rover002 wp000 wp003 )
    ( can_traverse rover002 wp000 wp004 )
    ( can_traverse rover002 wp000 wp005 )
    ( can_traverse rover002 wp000 wp007 )
    ( can_traverse rover002 wp001 wp000 )
    ( can_traverse rover002 wp001 wp006 )
    ( can_traverse rover002 wp002 wp001 )
    ( can_traverse rover002 wp002 wp006 )
    ( can_traverse rover002 wp002 wp007 )
    ( can_traverse rover002 wp002 wp008 )
    ( can_traverse rover002 wp002 wp009 )
    ( can_traverse rover002 wp003 wp000 )
    ( can_traverse rover002 wp003 wp002 )
    ( can_traverse rover002 wp003 wp005 )
    ( can_traverse rover002 wp003 wp011 )
    ( can_traverse rover002 wp003 wp013 )
    ( can_traverse rover002 wp003 wp014 )
    ( can_traverse rover002 wp004 wp000 )
    ( can_traverse rover002 wp004 wp001 )
    ( can_traverse rover002 wp004 wp008 )
    ( can_traverse rover002 wp004 wp012 )
    ( can_traverse rover002 wp004 wp014 )
    ( can_traverse rover002 wp005 wp001 )
    ( can_traverse rover002 wp005 wp003 )
    ( can_traverse rover002 wp005 wp004 )
    ( can_traverse rover002 wp005 wp007 )
    ( can_traverse rover002 wp005 wp012 )
    ( can_traverse rover002 wp006 wp000 )
    ( can_traverse rover002 wp006 wp002 )
    ( can_traverse rover002 wp006 wp003 )
    ( can_traverse rover002 wp006 wp004 )
    ( can_traverse rover002 wp006 wp011 )
    ( can_traverse rover002 wp006 wp014 )
    ( can_traverse rover002 wp007 wp001 )
    ( can_traverse rover002 wp007 wp003 )
    ( can_traverse rover002 wp007 wp005 )
    ( can_traverse rover002 wp007 wp006 )
    ( can_traverse rover002 wp007 wp010 )
    ( can_traverse rover002 wp008 wp000 )
    ( can_traverse rover002 wp008 wp004 )
    ( can_traverse rover002 wp008 wp005 )
    ( can_traverse rover002 wp008 wp009 )
    ( can_traverse rover002 wp008 wp012 )
    ( can_traverse rover002 wp009 wp003 )
    ( can_traverse rover002 wp009 wp008 )
    ( can_traverse rover002 wp009 wp010 )
    ( can_traverse rover002 wp009 wp013 )
    ( can_traverse rover002 wp010 wp002 )
    ( can_traverse rover002 wp010 wp004 )
    ( can_traverse rover002 wp010 wp006 )
    ( can_traverse rover002 wp010 wp008 )
    ( can_traverse rover002 wp011 wp000 )
    ( can_traverse rover002 wp011 wp003 )
    ( can_traverse rover002 wp011 wp004 )
    ( can_traverse rover002 wp011 wp006 )
    ( can_traverse rover002 wp011 wp008 )
    ( can_traverse rover002 wp011 wp009 )
    ( can_traverse rover002 wp011 wp010 )
    ( can_traverse rover002 wp011 wp014 )
    ( can_traverse rover002 wp012 wp005 )
    ( can_traverse rover002 wp012 wp011 )
    ( can_traverse rover002 wp012 wp014 )
    ( can_traverse rover002 wp013 wp000 )
    ( can_traverse rover002 wp013 wp001 )
    ( can_traverse rover002 wp013 wp003 )
    ( can_traverse rover002 wp013 wp008 )
    ( can_traverse rover002 wp013 wp010 )
    ( can_traverse rover002 wp014 wp001 )
    ( can_traverse rover002 wp014 wp002 )
    ( can_traverse rover002 wp014 wp003 )
    ( can_traverse rover002 wp014 wp005 )
    ( can_traverse rover002 wp014 wp011 )
    ( store_of store000 rover000 )
    ( store_of store001 rover001 )
    ( store_of store002 rover002 )
    ( empty store000 )
    ( empty store001 )
    ( empty store002 )
    ( visible_from obj000 wp006 )
    ( visible_from obj000 wp008 )
    ( visible_from obj000 wp012 )
    ( visible_from obj001 wp000 )
    ( visible_from obj001 wp006 )
    ( visible_from obj001 wp007 )
    ( visible_from obj001 wp010 )
    ( visible_from obj002 wp003 )
    ( visible_from obj002 wp005 )
    ( visible_from obj002 wp006 )
    ( visible_from obj002 wp008 )
    ( visible_from obj003 wp005 )
    ( visible_from obj004 wp002 )
    ( visible_from obj004 wp004 )
    ( visible_from obj004 wp006 )
    ( visible_from obj004 wp010 )
    ( visible_from obj004 wp011 )
    ( visible_from obj005 wp005 )
    ( visible_from obj005 wp007 )
    ( visible_from obj005 wp008 )
    ( visible_from obj005 wp011 )
    ( on_board cam000 rover000 )
    ( on_board cam001 rover000 )
    ( on_board cam002 rover000 )
    ( supports cam000 mode001 )
    ( supports cam000 mode002 )
    ( supports cam000 mode003 )
    ( supports cam001 mode002 )
    ( supports cam002 mode000 )
    ( supports cam002 mode002 )
    ( calibration_target cam000 obj000 )
    ( calibration_target cam001 obj003 )
    ( calibration_target cam002 obj004 )
  )
  ( :goal
    ( and
      ( communicated_soil_data wp003 )
      ( communicated_soil_data wp004 )
      ( communicated_soil_data wp009 )
      ( communicated_soil_data wp010 )
      ( communicated_soil_data wp012 )
      ( communicated_soil_data wp014 )
      ( communicated_rock_data wp004 )
      ( communicated_rock_data wp008 )
      ( communicated_rock_data wp009 )
      ( communicated_rock_data wp010 )
      ( communicated_rock_data wp011 )
      ( communicated_image_data obj000 mode001 )
      ( communicated_image_data obj001 mode001 )
      ( communicated_image_data obj002 mode000 )
      ( communicated_image_data obj003 mode003 )
      ( communicated_image_data obj004 mode001 )
      ( communicated_image_data obj005 mode000 )
    )
  )
)

