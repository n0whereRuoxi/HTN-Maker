( define ( htn-problem probname )
  ( :domain satellite )
  ( :requirements :strips :htn :typing :equality )
  ( :objects
    sat000 - satellite
    sat001 - satellite
    sat002 - satellite
    dir000 - direction
    dir001 - direction
    dir002 - direction
    dir003 - direction
    dir004 - direction
    mode000 - mode
    mode001 - mode
    mode002 - mode
    inst000-000 - instrument
    inst000-001 - instrument
    inst000-002 - instrument
    inst001-000 - instrument
    inst001-001 - instrument
    inst002-000 - instrument
  )

  ( :init
    ( pointing sat000 dir004 )
    ( pointing sat001 dir002 )
    ( pointing sat002 dir001 )
    ( power_avail sat000 )
    ( power_avail sat001 )
    ( power_avail sat002 )
    ( on_board inst000-000 sat000 )
    ( on_board inst000-001 sat000 )
    ( on_board inst000-002 sat000 )
    ( on_board inst001-000 sat001 )
    ( on_board inst001-001 sat001 )
    ( on_board inst002-000 sat002 )
    ( supports inst000-000 mode001 )
    ( supports inst000-001 mode002 )
    ( supports inst000-002 mode000 )
    ( supports inst001-000 mode000 )
    ( supports inst001-001 mode000 )
    ( supports inst002-000 mode000 )
    ( calibration_target inst000-000 dir000 )
    ( calibration_target inst000-001 dir000 )
    ( calibration_target inst000-002 dir000 )
    ( calibration_target inst001-000 dir002 )
    ( calibration_target inst001-001 dir003 )
    ( calibration_target inst002-000 dir001 )
    ( not_calibrated inst000-000 )
    ( not_calibrated inst000-001 )
    ( not_calibrated inst000-002 )
    ( not_calibrated inst001-000 )
    ( not_calibrated inst001-001 )
    ( not_calibrated inst002-000 )
  )
  ( :tasks
    ( GET_IMAGE dir001 mode002 )
    ( GET_IMAGE dir003 mode002 )
    ( GET_IMAGE dir000 mode000 )
    ( GET_IMAGE dir003 mode000 )
    ( GET_IMAGE dir004 mode002 )
  )
)

