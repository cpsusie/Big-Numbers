#pragma once

#include <MFCUtil/Viewport2D.h>
#include "Profile.h"

void paintProfile(const Profile &profile, Viewport2D &vp, COLORREF color);
void paintProfilePolygon(const ProfilePolygon &pp, Viewport2D &vp, COLORREF color);
void paintProfileNormals(const Profile &profile, Viewport2D &vp, COLORREF color, bool smooth);
