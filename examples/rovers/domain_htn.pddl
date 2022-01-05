; A domain to model NASA's Mars Rover project.

( define ( domain Rover )
  ( :requirements :typing :htn )
  ( :types
    rover      ; An automated device for collecting samples.
    waypoint   ; A location.
    store      ; A container on a rover.
    camera     ; A camera on a rover.
    mode       ; A mode of the camera sensor.
    lander     ; A lunar lander that can receive transmissions from rovers.
    objective  ; Something that can be photographed.
  )

  ( :predicates
      ; Rover ?r is at Waypoint ?y.
    ( at ?r - rover ?y - waypoint )
      ; Lander ?l is at Waypoint ?y.
    ( at_lander ?l - lander ?y - waypoint )
      ; Rover ?r can travel from Waypoint ?x to Waypoint ?y.
    ( can_traverse ?r - rover ?x - waypoint ?y - waypoint )
      ; Rover ?r can perform soil analysis.
    ( equipped_for_soil_analysis ?r - rover )
      ; Rover ?r can perform rock analysis.
    ( equipped_for_rock_analysis ?r - rover )
      ; Rover ?r can take images.
    ( equipped_for_imaging ?r - rover )
      ; Store ?s is empty (only alternative is full).
    ( empty ?s - store )
      ; Rover ?r has performed an analysis of rock from Waypoint ?w. 
    ( have_rock_analysis ?r - rover ?w - waypoint )
      ; Rover ?r has performed an analysis of soil from Waypoint ?w.
    ( have_soil_analysis ?r - rover ?w - waypoint )
      ; Store ?s is full (only alternative is empty).
    ( full ?s - store )
      ; Camera ?c on Rover ?r is calibrated.
    ( calibrated ?c - camera ?r - rover )
      ; Camera ?c supports Mode ?m.
    ( supports ?c - camera ?m - mode )
      ; Rover ?r is available for use.
      ; This is silly here, because no action removes this.
      ; Presumably it is important in a temporal version of the domain.
    ( available ?r - rover )
      ; Waypoing ?p is visible from Waypoint ?w.
    ( visible ?w - waypoint ?p - waypoint )
      ; Rover ?r has a Mode ?m image of Objective ?o.
    ( have_image ?r - rover ?o - objective ?m - mode )
      ; Soil data from Waypoint ?w has been relayed to a Lander.
    ( communicated_soil_data ?w - waypoint )
      ; Rock data from Waypoint ?w has been relayed to a Lander.
    ( communicated_rock_data ?w - waypoint )
      ; Image data of Mode ?m and Objective ?o has been relayed to a Lander.
    ( communicated_image_data ?o - objective ?m - mode )
      ; A soil sample exists at Waypoint ?w.
    ( at_soil_sample ?w - waypoint )
      ; A rock sample exists at Waypoint ?w.
    ( at_rock_sample ?w - waypoint )
      ; Objective ?o can be seen from Waypoint ?w.
    ( visible_from ?o - objective ?w - waypoint )
      ; Store ?s is located on Rover ?r.
    ( store_of ?s - store ?r - rover )
      ; Camera ?i can be calibrated when pointed at Objective ?o.
    ( calibration_target ?i - camera ?o - objective )
      ; Camera ?i is located on Rover ?r.
    ( on_board ?i - camera ?r - rover )
      ; Lander ?l has a free communications channel.
    ( channel_free ?l - lander )
  )


    ; Move Rover ?r from Waypoint ?y to Waypoint ?z.
  ( :action !Navigate
    :parameters
    (
      ?r - rover
      ?y - waypoint
      ?z - waypoint
    )
    :precondition
    ( and
      ( can_traverse ?r ?y ?z )
      ( available ?r )
      ( at ?r ?y )
      ( visible ?y ?z )
    )
    :effect
    ( and
      ( not ( at ?r ?y ) )
      ( at ?r ?z )
    )
  )

    ; Rover ?r takes a soil sample from Waypoint ?p, deposits it in Store ?s,
    ;   and analyzes it.
    ; This removes the soil sample from Waypoint ?p.
  ( :action !Sample_Soil
    :parameters
    (
      ?r - rover
      ?s - store
      ?p - waypoint
    )
    :precondition
    ( and
      ( at ?r ?p )
      ( at_soil_sample ?p )
      ( equipped_for_soil_analysis ?r )
      ( store_of ?s ?r )
      ( empty ?s )
    )
    :effect
    ( and
      ( not ( empty ?s ) )
      ( full ?s )
      ( have_soil_analysis ?r ?p )
      ( not ( at_soil_sample ?p ) )
    )
  )

    ; Rover ?r takes a rock sample from Waypoint ?p, deposits it in Store ?s,
    ;   and analyzes it.
    ; This removes the rock sample from Waypoint ?p.
  ( :action !Sample_Rock
    :parameters
    (
      ?r - rover
      ?s - store
      ?p - waypoint
    )
    :precondition
    ( and
      ( at ?r ?p )
      ( at_rock_sample ?p )
      ( equipped_for_rock_analysis ?r )
      ( store_of ?s ?r )
      ( empty ?s )
    )
    :effect
    ( and
      ( not ( empty ?s ) )
      ( full ?s )
      ( have_rock_analysis ?r ?p )
      ( not ( at_rock_sample ?p ) )
    )
  )

    ; Rover ?r drops whatever was in Store ?s.
  ( :action !Drop
    :parameters
    (
      ?r - rover
      ?s - store
    )
    :precondition
    ( and
      ( store_of ?s ?r )
      ( full ?s )
    )
    :effect
    ( and
      ( not ( full ?s ) )
      ( empty ?s )
    )
  )

    ; Camera ?i on Rover ?r at Waypoint ?w calibrates against Objective ?t.
  ( :action !Calibrate
    :parameters
    (
      ?r - rover
      ?i - camera
      ?t - objective
      ?w - waypoint
    )
    :precondition
    ( and
      ( equipped_for_imaging ?r )
      ( calibration_target ?i ?t )
      ( at ?r ?w )
      ( visible_from ?t ?w )
      ( on_board ?i ?r )
    )
    :effect
    ( and
      ( calibrated ?i ?r )
    )
  )

    ; Camera ?i on Rover ?r at Waypoint ?p takes a Mode ?m image of
    ;   Objective ?o.
  ( :action !Take_Image
    :parameters
    (
      ?r - rover
      ?p - waypoint
      ?o - objective
      ?i - camera
      ?m - mode
    )
    :precondition
    ( and
      ( calibrated ?i ?r )
      ( on_board ?i ?r )
      ( equipped_for_imaging ?r )
      ( supports ?i ?m )
      ( visible_from ?o ?p )
      ( at ?r ?p )
    )
    :effect
    ( and
      ( have_image ?r ?o ?m )
      ( not ( calibrated ?i ?r ) )
    )
  )

    ; Rover ?r at Wayoint ?x sends soil data from Waypoint ?p to Lander ?l at
    ;   Waypoint ?y.
    ; In the original version, this had ( available ?r ) and
    ;   ( channel_free ?l ) as both negative and positive effects.  I removed
    ;   the unnecessary negative effects because I was afraid they might
    ;   confuse HTN-Maker.
  ( :action !Communicate_Soil_Data
    :parameters
    (
      ?r - rover
      ?l - lander
      ?p - waypoint
      ?x - waypoint
      ?y - waypoint
    )
    :precondition
    ( and
      ( at ?r ?x )
      ( at_lander ?l ?y )
      ( have_soil_analysis ?r ?p )
      ( visible ?x ?y )
      ( available ?r )
      ( channel_free ?l )
    )
    :effect
    ( and
      ( channel_free ?l )
      ( communicated_soil_data ?p )
      ( available ?r )
    )
  )

    ; Rover ?r at Wayoint ?x sends rock data from Waypoint ?p to Lander ?l at
    ;   Waypoint ?y.
    ; In the original version, this had ( available ?r ) and
    ;   ( channel_free ?l ) as both negative and positive effects.  I removed
    ;   the unnecessary negative effects because I was afraid they might
    ;   confuse HTN-Maker.
  ( :action !Communicate_Rock_Data
    :parameters
    (
      ?r - rover
      ?l - lander
      ?p - waypoint
      ?x - waypoint
      ?y - waypoint
    )
    :precondition
    ( and
      ( at ?r ?x )
      ( at_lander ?l ?y )
      ( have_rock_analysis ?r ?p )
      ( visible ?x ?y )
      ( available ?r )
      ( channel_free ?l )
    )
    :effect
    ( and
      ( channel_free ?l )
      ( communicated_rock_data ?p )
      ( available ?r )
    )
  )

    ; Rover ?r at Waypoint ?x sends an image of Mode ?m Objective ?o to Lander
    ;   ?l at Waypoint ?y.
    ; In the original version, this had ( available ?r ) and
    ;   ( channel_free ?l ) as both negative and positive effects.  I removed
    ;   the unnecessary negative effects because I was afraid they might
    ;   confuse HTN-Maker.
  ( :action !Communicate_Image_Data
    :parameters
    (
      ?r - rover
      ?l - lander
      ?o - objective
      ?m - mode
      ?x - waypoint
      ?y - waypoint
    )
    :precondition
    ( and
      ( at ?r ?x )
      ( at_lander ?l ?y )
      ( have_image ?r ?o ?m )
      ( visible ?x ?y )
      ( available ?r )
      ( channel_free ?l )
    )
    :effect
    ( and
      ( channel_free ?l )
      ( communicated_image_data ?o ?m )
      ( available ?r )
    )
  )

  ( :method Get_Soil_Data
    :parameters
    (
      ?w - waypoint
    )
    :precondition
    ( and
      ( communicated_soil_data ?w )
    )
    :subtasks
    ( )
  )

  ( :method Get_Rock_Data
    :parameters
    (
      ?w - waypoint
    )
    :precondition
    ( and
      ( communicated_rock_data ?w )
    )
    :subtasks
    ( )
  )

  ( :method Get_Image_Data
    :parameters
    (
      ?o - objective
      ?m - mode
    )
    :precondition
    ( and
      ( communicated_image_data ?o ?m )
    )
    :subtasks
    ( )
  )

  ; Have sample, lander visible, so send.
  ( :method Get_Soil_Data
    :parameters
    (
      ?soil_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?lander - lander
      ?rover_loc - waypoint
      ?lander_loc - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( at_lander ?lander ?lander_loc )
      ( have_soil_analysis ?rover ?soil_loc )
      ( visible ?rover_loc ?lander_loc )
    )
    :subtasks
    (
      ( !Communicate_Soil_Data ?rover ?lander ?soil_loc ?rover_loc ?lander_loc )
    )
  )

  ; Have sample, need to move to sendable location.
  ( :method Get_Soil_Data
    :parameters
    (
      ?soil_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?lander - lander
      ?sendable_loc - waypoint
      ?lander_loc - waypoint
    )
    :precondition
    ( and
      ( at_lander ?lander ?lander_loc )
      ( have_soil_analysis ?rover ?soil_loc )
      ( visible ?sendable_loc ?lander_loc )
    )
    :subtasks
    (
      ( Travel_To ?rover ?sendable_loc )
      ( Get_Soil_Data ?soil_loc )
    )
  )

  ; Do not have sample, but in position to take it.
  ( :method Get_Soil_Data
    :parameters
    (
      ?soil_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?store - store
    )
    :precondition
    ( and
      ( at ?rover ?soil_loc )
      ( at_soil_sample ?soil_loc )
      ( equipped_for_soil_analysis ?rover )
      ( store_of ?store ?rover )
      ( empty ?store )
    )
    :subtasks
    (
      ( !Sample_Soil ?rover ?store ?soil_loc )
      ( Get_Soil_Data ?soil_loc )
    )
  )

  ; Could take sample except store is full.
  ( :method Get_Soil_Data
    :parameters
    (
      ?soil_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?store - store
    )
    :precondition
    ( and
      ( at ?rover ?soil_loc )
      ( at_soil_sample ?soil_loc )
      ( equipped_for_soil_analysis ?rover )
      ( store_of ?store ?rover )
      ( full ?store )
    )
    :subtasks
    (
      ( !Drop ?rover ?store )
      ( Get_Soil_Data ?soil_loc )
    )
  )

  ; Need to move to sample.
  ( :method Get_Soil_Data
    :parameters
    (
      ?soil_loc - waypoint
    )
    :vars
    (
      ?rover - rover
    )
    :precondition
    ( and
      ( at_soil_sample ?soil_loc )
      ( equipped_for_soil_analysis ?rover )
    )
    :subtasks
    (
      ( Travel_To ?rover ?soil_loc )
      ( Get_Soil_Data ?soil_loc )
    )
  )






  ; Have sample, lander visible, so send.
  ( :method Get_Rock_Data
    :parameters
    (
      ?rock_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?lander - lander
      ?rover_loc - waypoint
      ?lander_loc - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( at_lander ?lander ?lander_loc )
      ( have_rock_analysis ?rover ?rock_loc )
      ( visible ?rover_loc ?lander_loc )
    )
    :subtasks
    (
      ( !Communicate_Rock_Data ?rover ?lander ?rock_loc ?rover_loc ?lander_loc )
    )
  )

  ; Have sample, need to move to sendable location.
  ( :method Get_Rock_Data
    :parameters
    (
      ?rock_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?lander - lander
      ?sendable_loc - waypoint
      ?lander_loc - waypoint
    )
    :precondition
    ( and
      ( at_lander ?lander ?lander_loc )
      ( have_rock_analysis ?rover ?rock_loc )
      ( visible ?sendable_loc ?lander_loc )
    )
    :subtasks
    (
      ( Travel_To ?rover ?sendable_loc )
      ( Get_Rock_Data ?rock_loc )
    )
  )

  ; Do not have sample, but in position to take it.
  ( :method Get_Rock_Data
    :parameters
    (
      ?rock_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?store - store
    )
    :precondition
    ( and
      ( at ?rover ?rock_loc )
      ( at_rock_sample ?rock_loc )
      ( equipped_for_rock_analysis ?rover )
      ( store_of ?store ?rover )
      ( empty ?store )
    )
    :subtasks
    (
      ( !Sample_Rock ?rover ?store ?rock_loc )
      ( Get_Rock_Data ?rock_loc )
    )
  )

  ; Could take sample except store is full.
  ( :method Get_Rock_Data
    :parameters
    (
      ?rock_loc - waypoint
    )
    :vars
    (
      ?rover - rover
      ?store - store
    )
    :precondition
    ( and
      ( at ?rover ?rock_loc )
      ( at_rock_sample ?rock_loc )
      ( equipped_for_rock_analysis ?rover )
      ( store_of ?store ?rover )
      ( full ?store )
    )
    :subtasks
    (
      ( !Drop ?rover ?store )
      ( Get_Rock_Data ?rock_loc )
    )
  )

  ; Need to move to sample.
  ( :method Get_Rock_Data
    :parameters
    (
      ?rock_loc - waypoint
    )
    :vars
    (
      ?rover - rover
    )
    :precondition
    ( and
      ( at_rock_sample ?rock_loc )
      ( equipped_for_rock_analysis ?rover )
    )
    :subtasks
    (
      ( Travel_To ?rover ?rock_loc )
      ( Get_Rock_Data ?rock_loc )
    )
  )





  ; Have image, lander visible, so send.
  ( :method Get_Image_Data
    :parameters
    (
      ?goal - objective
      ?mode - mode
    )
    :vars
    (
      ?rover - rover
      ?lander - lander
      ?rover_loc - waypoint
      ?lander_loc - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( at_lander ?lander ?lander_loc )
      ( have_image ?rover ?goal ?mode )
      ( visible ?rover_loc ?lander_loc )
    )
    :subtasks
    (
      ( !Communicate_Image_Data ?rover ?lander ?goal ?mode ?rover_loc ?lander_loc )
    )
  )

  ; Have image, need to move to sendable location.
  ( :method Get_Image_Data
    :parameters
    (
      ?goal - objective
      ?mode - mode
    )
    :vars
    (
      ?rover - rover
      ?lander - lander
      ?sendable_loc - waypoint
      ?lander_loc - waypoint
    )
    :precondition
    ( and
      ( at_lander ?lander ?lander_loc )
      ( have_image ?rover ?goal ?mode )
      ( visible ?sendable_loc ?lander_loc )
    )
    :subtasks
    (
      ( Travel_To ?rover ?sendable_loc )
      ( Get_Image_Data ?goal ?mode )
    )
  )

  ; Ready to take image.
  ( :method Get_Image_Data
    :parameters
    (
      ?goal - objective
      ?mode - mode
    )
    :vars
    (
      ?rover - rover
      ?rover_loc - waypoint
      ?camera - camera
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( calibrated ?camera ?rover )
      ( on_board ?camera ?rover )
      ( equipped_for_imaging ?rover )
      ( supports ?camera ?mode )
      ( visible_from ?goal ?rover_loc )
    )
    :subtasks
    (
      ( !Take_Image ?rover ?rover_loc ?goal ?camera ?mode )
      ( Get_Image_Data ?goal ?mode )
    )
  )


  ; Need to move to take image.
  ( :method Get_Image_Data
    :parameters
    (
      ?goal - objective
      ?mode - mode
    )
    :vars
    (
      ?rover - rover
      ?camera - camera
      ?wp1 - waypoint
    )
    :precondition
    ( and
      ( calibrated ?camera ?rover )
      ( on_board ?camera ?rover )
      ( equipped_for_imaging ?rover )
      ( supports ?camera ?mode )
      ( visible_from ?goal ?wp1 )
    )
    :subtasks
    (
      ( Travel_To ?rover ?wp1 )
      ( Get_Image_Data ?goal ?mode )
    )
  )

  ; In position to calibrate.
  ( :method Get_Image_Data
    :parameters
    (
      ?goal - objective
      ?mode - mode
    )
    :vars
    (
      ?rover - rover
      ?camera - camera
      ?rover_loc - waypoint
      ?target - objective
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( on_board ?camera ?rover )
      ( equipped_for_imaging ?rover )
      ( supports ?camera ?mode )
      ( visible_from ?target ?rover_loc )
      ( calibration_target ?camera ?target )
    )
    :subtasks
    (
      ( !Calibrate ?rover ?camera ?target ?rover_loc )
      ( Get_Image_Data ?goal ?mode )
    )
  )

  ; Need to move to calibrate.
  ( :method Get_Image_Data
    :parameters
    (
      ?goal - objective
      ?mode - mode
    )
    :vars
    (
      ?rover - rover
      ?camera - camera
      ?visible_loc - waypoint
      ?target - objective
    )
    :precondition
    ( and
      ( on_board ?camera ?rover )
      ( equipped_for_imaging ?rover )
      ( supports ?camera ?mode )
      ( visible_from ?target ?visible_loc )
      ( calibration_target ?camera ?target )
    )
    :subtasks
    (
      ( Travel_To ?rover ?visible_loc )
      ( Get_Image_Data ?goal ?mode )
    )
  )






  ; Already there.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?end )
    )
    :subtasks
    ( )
  )

  ; Travel a path of length 1.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end  - waypoint
    )
    :vars
    (
      ?rover_loc - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( visible ?rover_loc ?end )
      ( can_traverse ?rover ?rover_loc ?end )
    )
    :subtasks
    (
      ( !Navigate ?rover ?rover_loc ?end )
    )
  )

  ; Travel a path of length 2.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end  - waypoint
    )
    :vars
    (
      ?rover_loc - waypoint
      ?wp1 - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( visible ?rover_loc ?wp1 )
      ( visible ?wp1 ?end )
      ( can_traverse ?rover ?rover_loc ?wp1 )
      ( can_traverse ?rover ?wp1 ?end )
    )
    :subtasks
    (
      ( !Navigate ?rover ?rover_loc ?wp1 )
      ( !Navigate ?rover ?wp1 ?end )
    )
  )

  ; Travel a path of length 3.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end  - waypoint
    )
    :vars
    (
      ?rover_loc - waypoint
      ?wp1 - waypoint
      ?wp2 - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( visible ?rover_loc ?wp1 )
      ( visible ?wp1 ?wp2 )
      ( visible ?wp2 ?end )
      ( can_traverse ?rover ?rover_loc ?wp1 )
      ( can_traverse ?rover ?wp1 ?wp2 )
      ( can_traverse ?rover ?wp2 ?end )
    )
    :subtasks
    (
      ( !Navigate ?rover ?rover_loc ?wp1 )
      ( !Navigate ?rover ?wp1 ?wp2 )
      ( !Navigate ?rover ?wp2 ?end )
    )
  )

  ; Travel a path of length 4.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end  - waypoint
    )
    :vars
    (
      ?rover_loc - waypoint
      ?wp1 - waypoint
      ?wp2 - waypoint
      ?wp3 - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( visible ?rover_loc ?wp1 )
      ( visible ?wp1 ?wp2 )
      ( visible ?wp2 ?wp3 )
      ( visible ?wp3 ?end )
      ( can_traverse ?rover ?rover_loc ?wp1 )
      ( can_traverse ?rover ?wp1 ?wp2 )
      ( can_traverse ?rover ?wp2 ?wp3 )
      ( can_traverse ?rover ?wp3 ?end )
    )
    :subtasks
    (
      ( !Navigate ?rover ?rover_loc ?wp1 )
      ( !Navigate ?rover ?wp1 ?wp2 )
      ( !Navigate ?rover ?wp2 ?wp3 )
      ( !Navigate ?rover ?wp3 ?end )
    )
  )

  ; Travel a path of length 5.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end  - waypoint
    )
    :vars
    (
      ?rover_loc - waypoint
      ?wp1 - waypoint
      ?wp2 - waypoint
      ?wp3 - waypoint
      ?wp4 - waypoint
    )
    :precondition
    ( and
      ( at ?rover ?rover_loc )
      ( visible ?rover_loc ?wp1 )
      ( visible ?wp1 ?wp2 )
      ( visible ?wp2 ?wp3 )
      ( visible ?wp3 ?wp4 )
      ( visible ?wp4 ?end )
      ( can_traverse ?rover ?rover_loc ?wp1 )
      ( can_traverse ?rover ?wp1 ?wp2 )
      ( can_traverse ?rover ?wp2 ?wp3 )
      ( can_traverse ?rover ?wp3 ?wp4 )
      ( can_traverse ?rover ?wp4 ?end )
    )
    :subtasks
    (
      ( !Navigate ?rover ?rover_loc ?wp1 )
      ( !Navigate ?rover ?wp1 ?wp2 )
      ( !Navigate ?rover ?wp2 ?wp3 )
      ( !Navigate ?rover ?wp3 ?wp4 )
      ( !Navigate ?rover ?wp4 ?end )
    )
  )

  ; General backwards-first search for a path of length greater than 5.
  ; This depends on random selection of possible instantiations to break out of
  ;   potential cycles.
  ( :method Travel_To
    :parameters
    (
      ?rover - rover
      ?end  - waypoint
    )
    :vars
    (
      ?wp1 - waypoint
    )
    :precondition
    ( and
      ( visible ?wp1 ?end )
      ( can_traverse ?rover ?wp1 ?end )
    )
    :subtasks
    (
      ( Travel_To ?rover ?wp1 )
      ( !Navigate ?rover ?wp1 ?end )
    )
  )


)
