( define ( problem probname )

  ( :domain logistics )

  ( :requirements :strips :typing :equality )

  ( :objects
    a000 - airplane
    a001 - airplane
    c000 - city
    c001 - city
    c002 - city
    c003 - city
    c004 - city
    c005 - city
    t000-000 - truck
    t001-000 - truck
    t001-001 - truck
    t002-000 - truck
    t003-000 - truck
    t004-000 - truck
    t004-001 - truck
    t005-000 - truck
    l000-000 - location
    l000-001 - location
    l000-002 - location
    l000-003 - location
    l001-000 - location
    l001-001 - location
    l001-002 - location
    l001-003 - location
    l001-004 - location
    l001-005 - location
    l002-000 - location
    l002-001 - location
    l002-002 - location
    l002-003 - location
    l002-004 - location
    l002-005 - location
    l003-000 - location
    l003-001 - location
    l003-002 - location
    l003-003 - location
    l004-000 - location
    l004-001 - location
    l004-002 - location
    l004-003 - location
    l005-000 - location
    l005-001 - location
    l005-002 - location
    l005-003 - location
    p000 - obj
    p001 - obj
    p002 - obj
    p003 - obj
    p004 - obj
    p005 - obj
    p006 - obj
    p007 - obj
    p008 - obj
    p009 - obj
    p010 - obj
    p011 - obj
    p012 - obj
    p013 - obj
    p014 - obj
    p015 - obj
    p016 - obj
    p017 - obj
    p018 - obj
    p019 - obj
  )

  ( :init
    ( in-city l000-000 c000 )
    ( in-city l000-001 c000 )
    ( in-city l000-002 c000 )
    ( in-city l000-003 c000 )
    ( in-city l001-000 c001 )
    ( in-city l001-001 c001 )
    ( in-city l001-002 c001 )
    ( in-city l001-003 c001 )
    ( in-city l001-004 c001 )
    ( in-city l001-005 c001 )
    ( in-city l002-000 c002 )
    ( in-city l002-001 c002 )
    ( in-city l002-002 c002 )
    ( in-city l002-003 c002 )
    ( in-city l002-004 c002 )
    ( in-city l002-005 c002 )
    ( in-city l003-000 c003 )
    ( in-city l003-001 c003 )
    ( in-city l003-002 c003 )
    ( in-city l003-003 c003 )
    ( in-city l004-000 c004 )
    ( in-city l004-001 c004 )
    ( in-city l004-002 c004 )
    ( in-city l004-003 c004 )
    ( in-city l005-000 c005 )
    ( in-city l005-001 c005 )
    ( in-city l005-002 c005 )
    ( in-city l005-003 c005 )
    ( airport l000-001 )
    ( airport l000-000 )
    ( airport l001-001 )
    ( airport l002-004 )
    ( airport l003-003 )
    ( airport l003-002 )
    ( airport l004-001 )
    ( airport l005-003 )
    ( truck-at t000-000 l000-001 )
    ( truck-at t001-000 l001-005 )
    ( truck-at t001-001 l001-004 )
    ( truck-at t002-000 l002-003 )
    ( truck-at t003-000 l003-002 )
    ( truck-at t004-000 l004-003 )
    ( truck-at t004-001 l004-000 )
    ( truck-at t005-000 l005-002 )
    ( airplane-at a000 l003-002 )
    ( airplane-at a001 l004-001 )
    ( obj-at p000 l001-003 )
    ( obj-at p001 l000-000 )
    ( obj-at p002 l001-004 )
    ( obj-at p003 l005-001 )
    ( obj-at p004 l004-003 )
    ( obj-at p005 l005-002 )
    ( obj-at p006 l002-005 )
    ( obj-at p007 l003-003 )
    ( obj-at p008 l002-003 )
    ( obj-at p009 l001-004 )
    ( obj-at p010 l002-005 )
    ( obj-at p011 l005-003 )
    ( obj-at p012 l001-004 )
    ( obj-at p013 l001-001 )
    ( obj-at p014 l001-001 )
    ( obj-at p015 l005-001 )
    ( obj-at p016 l000-001 )
    ( obj-at p017 l005-002 )
    ( obj-at p018 l002-004 )
    ( obj-at p019 l004-003 )
  )

  ( :goal
    ( and
      ( obj-at p000 l000-002 )
      ( obj-at p001 l001-001 )
      ( obj-at p002 l000-003 )
      ( obj-at p003 l003-002 )
      ( obj-at p004 l002-003 )
      ( obj-at p005 l001-004 )
      ( obj-at p006 l001-004 )
      ( obj-at p007 l001-002 )
      ( obj-at p008 l002-005 )
      ( obj-at p009 l001-003 )
      ( obj-at p010 l004-001 )
      ( obj-at p011 l005-002 )
      ( obj-at p012 l000-002 )
      ( obj-at p013 l004-000 )
      ( obj-at p014 l000-002 )
      ( obj-at p015 l002-001 )
      ( obj-at p016 l002-003 )
      ( obj-at p017 l004-002 )
      ( obj-at p018 l002-004 )
      ( obj-at p019 l002-003 )
    )
  )
)

