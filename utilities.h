//
// Created by bear on 10/19/2022.
//

#pragma once

#define MADE_IT(X) do { \
  std::cout << X << " -- @ " << __LINE__ << std::endl;   \
} while(false)


inline int CLAMP(int x, uint32_t lo, uint32_t hi) {
    return ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x));
}

