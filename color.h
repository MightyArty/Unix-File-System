#ifndef __COLOR__H__
#define __COLOR__H__
#include <stdio.h>
/**
 * @brief Colloring the text in red color
 * just for fun :)
 * @return none.
 */
#define RED printf("\033[1;31m")
/**
 * @brief Colloring the text in yellow color
 * just for fun :)
 * @return none.
 */
#define YELLOW printf("\033[1;33m")
/**
 * @brief Colloring the text in blue color
 * just for fun :)
 * @return none.
 */
#define BLUE printf("\033[1;34m")
/**
 * @brief Colloring the text in green color
 * just for fun :)
 * @return none.
 */
#define GREEN printf("\033[1;32m")
/**
 * @brief Colloring the text in cyan color
 * just for fun :)
 * @return none.
 */
#define CYAN printf("\033[1;36m")
/**
 * @brief Reseting the text color back to default
 * just for fun :)
 * @return none.
 */
#define RESET printf("\033[0m")

#endif //!__COLOR__H__