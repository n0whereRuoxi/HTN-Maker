( define ( problem probname )
  ( :domain satellite )
  ( :requirements :strips :typing :equality )

  ( :objects
    sat000 - satellite
    sat001 - satellite
    sat002 - satellite
    sat003 - satellite
    sat004 - satellite
    sat005 - satellite
    dir000 - direction
    dir001 - direction
    dir002 - direction
    dir003 - direction
    dir004 - direction
    dir005 - direction
    dir006 - direction
    dir007 - direction
    dir008 - direction
    dir009 - direction
    mode000 - mode
    mode001 - mode
    mode002 - mode
    mode003 - mode
    mode004 - mode
    inst000-000 - instrument
    inst000-001 - instrument
    inst000-002 - instrument
    inst000-003 - instrument
    inst000-004 - instrument
    inst001-000 - instrument
    inst001-001 - instrument
    inst001-002 - instrument
    inst001-003 - instrument
    inst002-000 - instrument
    inst002-001 - instrument
    inst003-000 - instrument
    inst003-001 - instrument
    inst003-002 - instrument
    inst003-003 - instrument
    inst003-004 - instrument
    inst004-000 - instrument
    inst004-001 - instrument
    inst005-000 - instrument
    inst005-001 - instrument
    inst005-002 - instrument
    inst005-003 - instrument
  )

  ( :init
    ( pointing sat000 dir002 )
    ( pointing sat001 dir000 )
    ( pointing sat002 dir009 )
    ( pointing sat003 dir002 )
    ( pointing sat004 dir008 )
    ( pointing sat005 dir003 )
    ( power_avail sat000 )
    ( power_avail sat001 )
    ( power_avail sat002 )
    ( power_avail sat003 )
    ( power_avail sat004 )
    ( power_avail sat005 )
    ( on_board inst000-000 sat000 )
    ( on_board inst000-001 sat000 )
    ( on_board inst000-002 sat000 )
    ( on_board inst000-003 sat000 )
    ( on_board inst000-004 sat000 )
    ( on_board inst001-000 sat001 )
    ( on_board inst001-001 sat001 )
    ( on_board inst001-002 sat001 )
    ( on_board inst001-003 sat001 )
    ( on_board inst002-000 sat002 )
    ( on_board inst002-001 sat002 )
    ( on_board inst003-000 sat003 )
    ( on_board inst003-001 sat003 )
    ( on_board inst003-002 sat003 )
    ( on_board inst003-003 sat003 )
    ( on_board inst003-004 sat003 )
    ( on_board inst004-000 sat004 )
    ( on_board inst004-001 sat004 )
    ( on_board inst005-000 sat005 )
    ( on_board inst005-001 sat005 )
    ( on_board inst005-002 sat005 )
    ( on_board inst005-003 sat005 )
    ( supports inst000-000 mode003 )
    ( supports inst000-001 mode004 )
    ( supports inst000-002 mode001 )
    ( supports inst000-003 mode002 )
    ( supports inst000-004 mode000 )
    ( supports inst001-000 mode004 )
    ( supports inst001-001 mode002 )
    ( supports inst001-002 mode001 )
    ( supports inst001-003 mode004 )
    ( supports inst002-000 mode002 )
    ( supports inst002-001 mode003 )
    ( supports inst003-000 mode001 )
    ( supports inst003-001 mode002 )
    ( supports inst003-002 mode000 )
    ( supports inst003-003 mode002 )
    ( supports inst003-004 mode000 )
    ( supports inst004-000 mode003 )
    ( supports inst004-001 mode003 )
    ( supports inst005-000 mode003 )
    ( supports inst005-001 mode000 )
    ( supports inst005-002 mode001 )
    ( supports inst005-003 mode003 )
    ( calibration_target inst000-000 dir007 )
    ( calibration_target inst000-001 dir004 )
    ( calibration_target inst000-002 dir001 )
    ( calibration_target inst000-003 dir004 )
    ( calibration_target inst000-004 dir001 )
    ( calibration_target inst001-000 dir006 )
    ( calibration_target inst001-001 dir000 )
    ( calibration_target inst001-002 dir005 )
    ( calibration_target inst001-003 dir005 )
    ( calibration_target inst002-000 dir005 )
    ( calibration_target inst002-001 dir000 )
    ( calibration_target inst003-000 dir000 )
    ( calibration_target inst003-001 dir002 )
    ( calibration_target inst003-002 dir006 )
    ( calibration_target inst003-003 dir009 )
    ( calibration_target inst003-004 dir008 )
    ( calibration_target inst004-000 dir008 )
    ( calibration_target inst004-001 dir009 )
    ( calibration_target inst005-000 dir004 )
    ( calibration_target inst005-001 dir000 )
    ( calibration_target inst005-002 dir002 )
    ( calibration_target inst005-003 dir009 )
    ( not_calibrated inst000-000 )
    ( not_calibrated inst000-001 )
    ( not_calibrated inst000-002 )
    ( not_calibrated inst000-003 )
    ( not_calibrated inst000-004 )
    ( not_calibrated inst001-000 )
    ( not_calibrated inst001-001 )
    ( not_calibrated inst001-002 )
    ( not_calibrated inst001-003 )
    ( not_calibrated inst002-000 )
    ( not_calibrated inst002-001 )
    ( not_calibrated inst003-000 )
    ( not_calibrated inst003-001 )
    ( not_calibrated inst003-002 )
    ( not_calibrated inst003-003 )
    ( not_calibrated inst003-004 )
    ( not_calibrated inst004-000 )
    ( not_calibrated inst004-001 )
    ( not_calibrated inst005-000 )
    ( not_calibrated inst005-001 )
    ( not_calibrated inst005-002 )
    ( not_calibrated inst005-003 )
  )
  ( :goal
    ( and
      ( have_image dir002 mode000 )
      ( have_image dir004 mode004 )
      ( have_image dir007 mode000 )
      ( have_image dir009 mode002 )
      ( have_image dir006 mode001 )
      ( have_image dir000 mode000 )
      ( have_image dir008 mode003 )
      ( have_image dir000 mode002 )
      ( have_image dir006 mode003 )
      ( have_image dir009 mode004 )
      ( have_image dir007 mode001 )
      ( have_image dir006 mode004 )
      ( have_image dir008 mode001 )
      ( have_image dir001 mode001 )
      ( have_image dir002 mode003 )
      ( have_image dir005 mode000 )
      ( have_image dir001 mode004 )
      ( have_image dir004 mode000 )
      ( have_image dir004 mode002 )
      ( have_image dir009 mode000 )
      ( have_image dir008 mode000 )
      ( have_image dir005 mode003 )
      ( have_image dir004 mode001 )
      ( have_image dir004 mode003 )
      ( have_image dir009 mode003 )
    )
  )
)

