/* This file is part of csgo-fake-opening by B3akers, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) b3akers 2020
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "csgo_fake_opening.hpp"
#include "sdk.hpp"
#include "item_manager.hpp"
#include "../fake-case-opening/hooks_f.hpp"

#include <thread>
#include <chrono>

void csgo_fake_opening::init( ) {
	AllocConsole( );
	freopen( "CONIN$", "r", stdin );
	freopen( "CONOUT$", "w", stdout );
	freopen( "CONOUT$", "w", stderr );

	sdk::init( );
	fhooks::hook( );
	item_manager::init( );

	while ( true ) {

		if ( GetAsyncKeyState( VK_F7 ) )
			break;

		using namespace std::chrono_literals;
		std::this_thread::sleep_for( 200ms );
	}

	fhooks::unhook( );

	FreeConsole( );

	FreeLibraryAndExitThread( my_module, 0 );
}

HMODULE csgo_fake_opening::my_module;