( define ( htn-problem probname )

  ( :domain logistics )

  ( :requirements :strips :htn :typing :equality )

  ( :objects
    a000 - airplane
    c000 - city
    c001 - city
    c002 - city
    t000-000 - truck
    t001-000 - truck
    t002-000 - truck
    l000-000 - location
    l000-001 - location
    l000-002 - location
    l001-000 - location
    l001-001 - location
    l001-002 - location
    l002-000 - location
    l002-001 - location
    l002-002 - location
    p000 - obj
    p001 - obj
    p002 - obj
    p003 - obj
    p004 - obj
  )

  ( :init
    ( in-city l000-000 c000 )
    ( in-city l000-001 c000 )
    ( in-city l000-002 c000 )
    ( in-city l001-000 c001 )
    ( in-city l001-001 c001 )
    ( in-city l001-002 c001 )
    ( in-city l002-000 c002 )
    ( in-city l002-001 c002 )
    ( in-city l002-002 c002 )
    ( airport l000-002 )
    ( airport l001-002 )
    ( airport l002-001 )
    ( truck-at t000-000 l000-002 )
    ( truck-at t001-000 l001-002 )
    ( truck-at t002-000 l002-000 )
    ( airplane-at a000 l000-002 )
    ( obj-at p000 l000-002 )
    ( obj-at p001 l002-002 )
    ( obj-at p002 l001-000 )
    ( obj-at p003 l001-002 )
    ( obj-at p004 l002-000 )
  )

  ( :tasks
    ( DELIVER-PKG p000 l002-001 )
    ( DELIVER-PKG p001 l002-001 )
    ( DELIVER-PKG p002 l001-002 )
    ( DELIVER-PKG p003 l001-000 )
    ( DELIVER-PKG p004 l002-001 )
  )
)

