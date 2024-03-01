// -----------------------------------------------------------------
// mandelbrot256 - a command line program able to render Mandelbrot set 
//                on terminal supporting ANSI ecape sequences.
// Copyright (C) 2024  Gabriele Bonacini
//
// This program is free software for no profit use, then you can redistribute 
// it and/or modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// A commercial license is also available for a lucrative use.
// -----------------------------------------------------------------

 #include <iostream>
 #include <algorithm>
 #include <random>
 #include <chrono>

 #include <mandelbrot256.hpp>

namespace mandelbrot256 {

 using std::chrono::system_clock,
       std::default_random_engine,
       std::shuffle,
       std::abs,
       std::cout;
       
 
 MandelbrotShell::MandelbrotShell(int w, int h, int z, int dx, int dy, int max)  noexcept
     : width{w}, height{h}, zoom{z}, deltax{ dx }, deltay{ dy }, pixels{w * h}, maxiter{max}, 
       center(-0.8 + deltax / 10.0, 0.0 + deltay / 10.0), span(2.7/zoom, -(5/3.0)*2.7*height/width/zoom), begin { center - span/2.0}
 {}

 void MandelbrotShell::setFullColours(void) noexcept {
     long seed { system_clock::now().time_since_epoch().count() };
     default_random_engine e(seed);
     palette.resize(256);
     for(unsigned int i{0x0}; i<= 0xFF; i++)
        palette[i]=static_cast<unsigned char>(i);

     shuffle(palette.begin(), palette.end(), e);     
 }

 void  MandelbrotShell::setWidth(int ww) noexcept{
     width=ww;
 }

 void  MandelbrotShell::setHeight(int hh) noexcept{
     height=hh;
 }

 void  MandelbrotShell::setMaxiter(int max) noexcept{
     maxiter=max;
 }

 void  MandelbrotShell::setZoom(int zz) noexcept{
     zoom=zz;
 }

 void  MandelbrotShell::setDeltaX(int dx) noexcept{
     deltax=dx;
 }

 void  MandelbrotShell::setDeltaY(int dy) noexcept{
     deltay=dy;
 }

 void MandelbrotShell::setCustomPalette(PaletteInit init)  noexcept{
     if(init.size() >= MIN_PALETTE_LEN && init.size() <= 0xFF + 1) palette = init;
 }

 void MandelbrotShell::render(void) const noexcept{
     auto calculate { [](Complex cmp, int maxiter) -> int {
                           int  n { 0 };
                           for(Complex z { cmp }; n < maxiter; n++) {
                               if( abs(z) >= 2.0) break;
                               z = z * z + cmp;
                           }
                           return n == maxiter ? 0 : n;
                      }
                     };

     #pragma omp parallel for ordered schedule(dynamic)
     for(int pix = 0 ; pix < pixels; ++pix) {
         const int x { pix % width }, 
                   y { pix / width };
         
         Complex   c { begin + Complex(x * span.real() / (width + 1.0),
                       y * span.imag() / (height + 1.0)) };
         
         int n { calculate(c, maxiter) };

         #pragma omp ordered
         {
           unsigned char colour { n > 0 ? palette[n % (palette.size() - 1)] : static_cast<unsigned char>(0x00) };

           cout << "\033[48;5;" << static_cast<int>(colour) << "m \033[m";
           if(x + 1 == width) cout << '\n';
         }
     }
 }

 } //End Namespace
