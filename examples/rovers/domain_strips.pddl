; A domain to model NASA's Mars Rover project.

( define ( domain Rover )
  ( :requirements :typing )
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

)
