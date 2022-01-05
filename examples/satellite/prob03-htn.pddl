( define ( htn-problem probname )
  ( :domain satellite )
  ( :requirements :strips :htn :typing :equality )
  ( :objects
    sat000 - satellite
    sat001 - satellite
    sat002 - satellite
    sat003 - satellite
    sat004 - satellite
    sat005 - satellite
    sat006 - satellite
    sat007 - satellite
    sat008 - satellite
    sat009 - satellite
    sat010 - satellite
    sat011 - satellite
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
    dir010 - direction
    dir011 - direction
    dir012 - direction
    dir013 - direction
    dir014 - direction
    dir015 - direction
    dir016 - direction
    dir017 - direction
    dir018 - direction
    dir019 - direction
    mode000 - mode
    mode001 - mode
    mode002 - mode
    mode003 - mode
    mode004 - mode
    mode005 - mode
    mode006 - mode
    mode007 - mode
    mode008 - mode
    mode009 - mode
    inst000-000 - instrument
    inst000-001 - instrument
    inst000-002 - instrument
    inst000-003 - instrument
    inst000-004 - instrument
    inst000-005 - instrument
    inst001-000 - instrument
    inst001-001 - instrument
    inst001-002 - instrument
    inst001-003 - instrument
    inst001-004 - instrument
    inst001-005 - instrument
    inst001-006 - instrument
    inst002-000 - instrument
    inst002-001 - instrument
    inst002-002 - instrument
    inst002-003 - instrument
    inst002-004 - instrument
    inst002-005 - instrument
    inst002-006 - instrument
    inst002-007 - instrument
    inst003-000 - instrument
    inst003-001 - instrument
    inst003-002 - instrument
    inst003-003 - instrument
    inst004-000 - instrument
    inst004-001 - instrument
    inst004-002 - instrument
    inst004-003 - instrument
    inst004-004 - instrument
    inst005-000 - instrument
    inst005-001 - instrument
    inst005-002 - instrument
    inst005-003 - instrument
    inst005-004 - instrument
    inst005-005 - instrument
    inst005-006 - instrument
    inst006-000 - instrument
    inst006-001 - instrument
    inst006-002 - instrument
    inst006-003 - instrument
    inst007-000 - instrument
    inst007-001 - instrument
    inst007-002 - instrument
    inst007-003 - instrument
    inst007-004 - instrument
    inst007-005 - instrument
    inst008-000 - instrument
    inst008-001 - instrument
    inst008-002 - instrument
    inst008-003 - instrument
    inst008-004 - instrument
    inst008-005 - instrument
    inst009-000 - instrument
    inst009-001 - instrument
    inst009-002 - instrument
    inst009-003 - instrument
    inst009-004 - instrument
    inst009-005 - instrument
    inst009-006 - instrument
    inst009-007 - instrument
    inst010-000 - instrument
    inst010-001 - instrument
    inst010-002 - instrument
    inst010-003 - instrument
    inst011-000 - instrument
    inst011-001 - instrument
    inst011-002 - instrument
    inst011-003 - instrument
  )

  ( :init
    ( pointing sat000 dir006 )
    ( pointing sat001 dir003 )
    ( pointing sat002 dir010 )
    ( pointing sat003 dir005 )
    ( pointing sat004 dir008 )
    ( pointing sat005 dir010 )
    ( pointing sat006 dir009 )
    ( pointing sat007 dir015 )
    ( pointing sat008 dir016 )
    ( pointing sat009 dir019 )
    ( pointing sat010 dir012 )
    ( pointing sat011 dir017 )
    ( power_avail sat000 )
    ( power_avail sat001 )
    ( power_avail sat002 )
    ( power_avail sat003 )
    ( power_avail sat004 )
    ( power_avail sat005 )
    ( power_avail sat006 )
    ( power_avail sat007 )
    ( power_avail sat008 )
    ( power_avail sat009 )
    ( power_avail sat010 )
    ( power_avail sat011 )
    ( on_board inst000-000 sat000 )
    ( on_board inst000-001 sat000 )
    ( on_board inst000-002 sat000 )
    ( on_board inst000-003 sat000 )
    ( on_board inst000-004 sat000 )
    ( on_board inst000-005 sat000 )
    ( on_board inst001-000 sat001 )
    ( on_board inst001-001 sat001 )
    ( on_board inst001-002 sat001 )
    ( on_board inst001-003 sat001 )
    ( on_board inst001-004 sat001 )
    ( on_board inst001-005 sat001 )
    ( on_board inst001-006 sat001 )
    ( on_board inst002-000 sat002 )
    ( on_board inst002-001 sat002 )
    ( on_board inst002-002 sat002 )
    ( on_board inst002-003 sat002 )
    ( on_board inst002-004 sat002 )
    ( on_board inst002-005 sat002 )
    ( on_board inst002-006 sat002 )
    ( on_board inst002-007 sat002 )
    ( on_board inst003-000 sat003 )
    ( on_board inst003-001 sat003 )
    ( on_board inst003-002 sat003 )
    ( on_board inst003-003 sat003 )
    ( on_board inst004-000 sat004 )
    ( on_board inst004-001 sat004 )
    ( on_board inst004-002 sat004 )
    ( on_board inst004-003 sat004 )
    ( on_board inst004-004 sat004 )
    ( on_board inst005-000 sat005 )
    ( on_board inst005-001 sat005 )
    ( on_board inst005-002 sat005 )
    ( on_board inst005-003 sat005 )
    ( on_board inst005-004 sat005 )
    ( on_board inst005-005 sat005 )
    ( on_board inst005-006 sat005 )
    ( on_board inst006-000 sat006 )
    ( on_board inst006-001 sat006 )
    ( on_board inst006-002 sat006 )
    ( on_board inst006-003 sat006 )
    ( on_board inst007-000 sat007 )
    ( on_board inst007-001 sat007 )
    ( on_board inst007-002 sat007 )
    ( on_board inst007-003 sat007 )
    ( on_board inst007-004 sat007 )
    ( on_board inst007-005 sat007 )
    ( on_board inst008-000 sat008 )
    ( on_board inst008-001 sat008 )
    ( on_board inst008-002 sat008 )
    ( on_board inst008-003 sat008 )
    ( on_board inst008-004 sat008 )
    ( on_board inst008-005 sat008 )
    ( on_board inst009-000 sat009 )
    ( on_board inst009-001 sat009 )
    ( on_board inst009-002 sat009 )
    ( on_board inst009-003 sat009 )
    ( on_board inst009-004 sat009 )
    ( on_board inst009-005 sat009 )
    ( on_board inst009-006 sat009 )
    ( on_board inst009-007 sat009 )
    ( on_board inst010-000 sat010 )
    ( on_board inst010-001 sat010 )
    ( on_board inst010-002 sat010 )
    ( on_board inst010-003 sat010 )
    ( on_board inst011-000 sat011 )
    ( on_board inst011-001 sat011 )
    ( on_board inst011-002 sat011 )
    ( on_board inst011-003 sat011 )
    ( supports inst000-000 mode002 )
    ( supports inst000-001 mode001 )
    ( supports inst000-002 mode009 )
    ( supports inst000-003 mode000 )
    ( supports inst000-004 mode007 )
    ( supports inst000-005 mode004 )
    ( supports inst001-000 mode005 )
    ( supports inst001-001 mode006 )
    ( supports inst001-002 mode008 )
    ( supports inst001-003 mode003 )
    ( supports inst001-004 mode009 )
    ( supports inst001-005 mode005 )
    ( supports inst001-006 mode001 )
    ( supports inst002-000 mode003 )
    ( supports inst002-001 mode006 )
    ( supports inst002-002 mode003 )
    ( supports inst002-003 mode004 )
    ( supports inst002-004 mode000 )
    ( supports inst002-005 mode009 )
    ( supports inst002-006 mode007 )
    ( supports inst002-007 mode005 )
    ( supports inst003-000 mode006 )
    ( supports inst003-001 mode002 )
    ( supports inst003-002 mode006 )
    ( supports inst003-003 mode001 )
    ( supports inst004-000 mode007 )
    ( supports inst004-001 mode003 )
    ( supports inst004-002 mode006 )
    ( supports inst004-003 mode006 )
    ( supports inst004-004 mode004 )
    ( supports inst005-000 mode002 )
    ( supports inst005-001 mode000 )
    ( supports inst005-002 mode006 )
    ( supports inst005-003 mode005 )
    ( supports inst005-004 mode005 )
    ( supports inst005-005 mode004 )
    ( supports inst005-006 mode003 )
    ( supports inst006-000 mode007 )
    ( supports inst006-001 mode000 )
    ( supports inst006-002 mode003 )
    ( supports inst006-003 mode002 )
    ( supports inst007-000 mode008 )
    ( supports inst007-001 mode004 )
    ( supports inst007-002 mode009 )
    ( supports inst007-003 mode002 )
    ( supports inst007-004 mode007 )
    ( supports inst007-005 mode003 )
    ( supports inst008-000 mode000 )
    ( supports inst008-001 mode008 )
    ( supports inst008-002 mode008 )
    ( supports inst008-003 mode009 )
    ( supports inst008-004 mode006 )
    ( supports inst008-005 mode005 )
    ( supports inst009-000 mode003 )
    ( supports inst009-001 mode003 )
    ( supports inst009-002 mode004 )
    ( supports inst009-003 mode004 )
    ( supports inst009-004 mode009 )
    ( supports inst009-005 mode008 )
    ( supports inst009-006 mode006 )
    ( supports inst009-007 mode005 )
    ( supports inst010-000 mode002 )
    ( supports inst010-001 mode003 )
    ( supports inst010-002 mode000 )
    ( supports inst010-003 mode003 )
    ( supports inst011-000 mode002 )
    ( supports inst011-001 mode000 )
    ( supports inst011-002 mode001 )
    ( supports inst011-003 mode007 )
    ( calibration_target inst000-000 dir018 )
    ( calibration_target inst000-001 dir007 )
    ( calibration_target inst000-002 dir017 )
    ( calibration_target inst000-003 dir005 )
    ( calibration_target inst000-004 dir012 )
    ( calibration_target inst000-005 dir010 )
    ( calibration_target inst001-000 dir015 )
    ( calibration_target inst001-001 dir015 )
    ( calibration_target inst001-002 dir019 )
    ( calibration_target inst001-003 dir010 )
    ( calibration_target inst001-004 dir019 )
    ( calibration_target inst001-005 dir010 )
    ( calibration_target inst001-006 dir002 )
    ( calibration_target inst002-000 dir018 )
    ( calibration_target inst002-001 dir001 )
    ( calibration_target inst002-002 dir001 )
    ( calibration_target inst002-003 dir011 )
    ( calibration_target inst002-004 dir015 )
    ( calibration_target inst002-005 dir013 )
    ( calibration_target inst002-006 dir002 )
    ( calibration_target inst002-007 dir013 )
    ( calibration_target inst003-000 dir015 )
    ( calibration_target inst003-001 dir003 )
    ( calibration_target inst003-002 dir004 )
    ( calibration_target inst003-003 dir017 )
    ( calibration_target inst004-000 dir011 )
    ( calibration_target inst004-001 dir001 )
    ( calibration_target inst004-002 dir016 )
    ( calibration_target inst004-003 dir001 )
    ( calibration_target inst004-004 dir010 )
    ( calibration_target inst005-000 dir019 )
    ( calibration_target inst005-001 dir010 )
    ( calibration_target inst005-002 dir003 )
    ( calibration_target inst005-003 dir001 )
    ( calibration_target inst005-004 dir012 )
    ( calibration_target inst005-005 dir003 )
    ( calibration_target inst005-006 dir009 )
    ( calibration_target inst006-000 dir007 )
    ( calibration_target inst006-001 dir019 )
    ( calibration_target inst006-002 dir002 )
    ( calibration_target inst006-003 dir006 )
    ( calibration_target inst007-000 dir004 )
    ( calibration_target inst007-001 dir009 )
    ( calibration_target inst007-002 dir003 )
    ( calibration_target inst007-003 dir017 )
    ( calibration_target inst007-004 dir013 )
    ( calibration_target inst007-005 dir015 )
    ( calibration_target inst008-000 dir005 )
    ( calibration_target inst008-001 dir012 )
    ( calibration_target inst008-002 dir009 )
    ( calibration_target inst008-003 dir016 )
    ( calibration_target inst008-004 dir009 )
    ( calibration_target inst008-005 dir006 )
    ( calibration_target inst009-000 dir005 )
    ( calibration_target inst009-001 dir018 )
    ( calibration_target inst009-002 dir016 )
    ( calibration_target inst009-003 dir003 )
    ( calibration_target inst009-004 dir006 )
    ( calibration_target inst009-005 dir003 )
    ( calibration_target inst009-006 dir004 )
    ( calibration_target inst009-007 dir019 )
    ( calibration_target inst010-000 dir008 )
    ( calibration_target inst010-001 dir017 )
    ( calibration_target inst010-002 dir016 )
    ( calibration_target inst010-003 dir017 )
    ( calibration_target inst011-000 dir008 )
    ( calibration_target inst011-001 dir011 )
    ( calibration_target inst011-002 dir017 )
    ( calibration_target inst011-003 dir000 )
    ( not_calibrated inst000-000 )
    ( not_calibrated inst000-001 )
    ( not_calibrated inst000-002 )
    ( not_calibrated inst000-003 )
    ( not_calibrated inst000-004 )
    ( not_calibrated inst000-005 )
    ( not_calibrated inst001-000 )
    ( not_calibrated inst001-001 )
    ( not_calibrated inst001-002 )
    ( not_calibrated inst001-003 )
    ( not_calibrated inst001-004 )
    ( not_calibrated inst001-005 )
    ( not_calibrated inst001-006 )
    ( not_calibrated inst002-000 )
    ( not_calibrated inst002-001 )
    ( not_calibrated inst002-002 )
    ( not_calibrated inst002-003 )
    ( not_calibrated inst002-004 )
    ( not_calibrated inst002-005 )
    ( not_calibrated inst002-006 )
    ( not_calibrated inst002-007 )
    ( not_calibrated inst003-000 )
    ( not_calibrated inst003-001 )
    ( not_calibrated inst003-002 )
    ( not_calibrated inst003-003 )
    ( not_calibrated inst004-000 )
    ( not_calibrated inst004-001 )
    ( not_calibrated inst004-002 )
    ( not_calibrated inst004-003 )
    ( not_calibrated inst004-004 )
    ( not_calibrated inst005-000 )
    ( not_calibrated inst005-001 )
    ( not_calibrated inst005-002 )
    ( not_calibrated inst005-003 )
    ( not_calibrated inst005-004 )
    ( not_calibrated inst005-005 )
    ( not_calibrated inst005-006 )
    ( not_calibrated inst006-000 )
    ( not_calibrated inst006-001 )
    ( not_calibrated inst006-002 )
    ( not_calibrated inst006-003 )
    ( not_calibrated inst007-000 )
    ( not_calibrated inst007-001 )
    ( not_calibrated inst007-002 )
    ( not_calibrated inst007-003 )
    ( not_calibrated inst007-004 )
    ( not_calibrated inst007-005 )
    ( not_calibrated inst008-000 )
    ( not_calibrated inst008-001 )
    ( not_calibrated inst008-002 )
    ( not_calibrated inst008-003 )
    ( not_calibrated inst008-004 )
    ( not_calibrated inst008-005 )
    ( not_calibrated inst009-000 )
    ( not_calibrated inst009-001 )
    ( not_calibrated inst009-002 )
    ( not_calibrated inst009-003 )
    ( not_calibrated inst009-004 )
    ( not_calibrated inst009-005 )
    ( not_calibrated inst009-006 )
    ( not_calibrated inst009-007 )
    ( not_calibrated inst010-000 )
    ( not_calibrated inst010-001 )
    ( not_calibrated inst010-002 )
    ( not_calibrated inst010-003 )
    ( not_calibrated inst011-000 )
    ( not_calibrated inst011-001 )
    ( not_calibrated inst011-002 )
    ( not_calibrated inst011-003 )
  )
  ( :tasks
    ( GET_IMAGE dir014 mode001 )
    ( GET_IMAGE dir001 mode006 )
    ( GET_IMAGE dir004 mode001 )
    ( GET_IMAGE dir019 mode006 )
    ( GET_IMAGE dir010 mode005 )
    ( GET_IMAGE dir013 mode005 )
    ( GET_IMAGE dir003 mode004 )
    ( GET_IMAGE dir009 mode002 )
    ( GET_IMAGE dir004 mode008 )
    ( GET_IMAGE dir005 mode002 )
    ( GET_IMAGE dir006 mode003 )
    ( GET_IMAGE dir006 mode009 )
    ( GET_IMAGE dir014 mode003 )
    ( GET_IMAGE dir012 mode001 )
    ( GET_IMAGE dir010 mode002 )
    ( GET_IMAGE dir009 mode008 )
    ( GET_IMAGE dir005 mode008 )
    ( GET_IMAGE dir005 mode003 )
    ( GET_IMAGE dir000 mode002 )
    ( GET_IMAGE dir012 mode006 )
    ( GET_IMAGE dir007 mode005 )
    ( GET_IMAGE dir002 mode004 )
    ( GET_IMAGE dir007 mode006 )
    ( GET_IMAGE dir011 mode004 )
    ( GET_IMAGE dir007 mode009 )
    ( GET_IMAGE dir017 mode000 )
    ( GET_IMAGE dir008 mode004 )
    ( GET_IMAGE dir007 mode002 )
    ( GET_IMAGE dir011 mode007 )
    ( GET_IMAGE dir019 mode009 )
    ( GET_IMAGE dir012 mode009 )
    ( GET_IMAGE dir004 mode000 )
    ( GET_IMAGE dir001 mode003 )
    ( GET_IMAGE dir004 mode004 )
    ( GET_IMAGE dir018 mode009 )
    ( GET_IMAGE dir016 mode008 )
    ( GET_IMAGE dir011 mode008 )
    ( GET_IMAGE dir004 mode003 )
    ( GET_IMAGE dir008 mode006 )
    ( GET_IMAGE dir010 mode003 )
    ( GET_IMAGE dir002 mode009 )
    ( GET_IMAGE dir015 mode003 )
    ( GET_IMAGE dir011 mode005 )
    ( GET_IMAGE dir004 mode007 )
    ( GET_IMAGE dir012 mode004 )
    ( GET_IMAGE dir018 mode001 )
    ( GET_IMAGE dir003 mode007 )
    ( GET_IMAGE dir007 mode003 )
    ( GET_IMAGE dir018 mode006 )
    ( GET_IMAGE dir006 mode008 )
    ( GET_IMAGE dir016 mode002 )
    ( GET_IMAGE dir011 mode009 )
    ( GET_IMAGE dir014 mode009 )
    ( GET_IMAGE dir006 mode000 )
    ( GET_IMAGE dir015 mode001 )
    ( GET_IMAGE dir019 mode000 )
    ( GET_IMAGE dir000 mode008 )
    ( GET_IMAGE dir013 mode002 )
    ( GET_IMAGE dir014 mode004 )
    ( GET_IMAGE dir010 mode007 )
    ( GET_IMAGE dir009 mode003 )
    ( GET_IMAGE dir010 mode008 )
    ( GET_IMAGE dir014 mode002 )
    ( GET_IMAGE dir000 mode004 )
    ( GET_IMAGE dir013 mode000 )
    ( GET_IMAGE dir008 mode008 )
    ( GET_IMAGE dir003 mode001 )
    ( GET_IMAGE dir001 mode001 )
    ( GET_IMAGE dir016 mode004 )
    ( GET_IMAGE dir017 mode001 )
    ( GET_IMAGE dir018 mode004 )
    ( GET_IMAGE dir015 mode009 )
    ( GET_IMAGE dir012 mode008 )
    ( GET_IMAGE dir012 mode005 )
    ( GET_IMAGE dir018 mode000 )
    ( GET_IMAGE dir015 mode008 )
    ( GET_IMAGE dir015 mode007 )
    ( GET_IMAGE dir019 mode007 )
    ( GET_IMAGE dir013 mode007 )
    ( GET_IMAGE dir003 mode008 )
    ( GET_IMAGE dir005 mode005 )
    ( GET_IMAGE dir019 mode008 )
    ( GET_IMAGE dir004 mode005 )
    ( GET_IMAGE dir005 mode000 )
    ( GET_IMAGE dir008 mode001 )
    ( GET_IMAGE dir004 mode002 )
    ( GET_IMAGE dir009 mode007 )
    ( GET_IMAGE dir007 mode001 )
    ( GET_IMAGE dir001 mode005 )
    ( GET_IMAGE dir002 mode008 )
    ( GET_IMAGE dir003 mode000 )
    ( GET_IMAGE dir016 mode000 )
    ( GET_IMAGE dir005 mode006 )
    ( GET_IMAGE dir000 mode000 )
    ( GET_IMAGE dir005 mode009 )
    ( GET_IMAGE dir011 mode006 )
    ( GET_IMAGE dir015 mode005 )
    ( GET_IMAGE dir011 mode001 )
    ( GET_IMAGE dir017 mode006 )
    ( GET_IMAGE dir013 mode008 )
    ( GET_IMAGE dir009 mode006 )
    ( GET_IMAGE dir001 mode002 )
    ( GET_IMAGE dir013 mode004 )
    ( GET_IMAGE dir014 mode005 )
    ( GET_IMAGE dir016 mode001 )
    ( GET_IMAGE dir012 mode003 )
    ( GET_IMAGE dir014 mode000 )
    ( GET_IMAGE dir016 mode009 )
    ( GET_IMAGE dir017 mode005 )
    ( GET_IMAGE dir000 mode006 )
    ( GET_IMAGE dir018 mode008 )
    ( GET_IMAGE dir018 mode005 )
    ( GET_IMAGE dir007 mode000 )
    ( GET_IMAGE dir013 mode009 )
    ( GET_IMAGE dir008 mode009 )
    ( GET_IMAGE dir000 mode001 )
    ( GET_IMAGE dir000 mode003 )
    ( GET_IMAGE dir004 mode006 )
    ( GET_IMAGE dir007 mode004 )
    ( GET_IMAGE dir017 mode008 )
    ( GET_IMAGE dir010 mode001 )
    ( GET_IMAGE dir001 mode000 )
    ( GET_IMAGE dir002 mode001 )
    ( GET_IMAGE dir011 mode003 )
    ( GET_IMAGE dir005 mode004 )
  )
)

