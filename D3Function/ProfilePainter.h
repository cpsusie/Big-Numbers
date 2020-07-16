#pragma once

class Profile2D;
class ProfilePolygon2D;
class Viewport2D;

void paintProfile(       const Profile2D        &profile, Viewport2D &vp, COLORREF color);
void paintProfilePolygon(const ProfilePolygon2D &pp     , Viewport2D &vp, COLORREF color);
void paintProfileNormals(const Profile2D        &profile, Viewport2D &vp, COLORREF color, bool smooth);
