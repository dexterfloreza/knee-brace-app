Yaw data plot code
Dataset_sub <- Dataset[seq(1, nrow(Dataset), 5), ]
par(mfrow = c(2, 3), mar = c(4, 4, 2, 1))
plot(Dataset_sub$WallTime, Dataset_sub[[2]], type = "l", col = "blue",
main = "Yaw (No arthritis set 1)", xlab = "Time (s)", ylab = "Yaw")
plot(Dataset_sub$WallTime, Dataset_sub[[3]], type = "l", col = "darkgreen",
main = "Yaw (No arthritis set 2)", xlab = "Time (s)", ylab = "Yaw")
plot(Dataset_sub$WallTime, Dataset_sub[[4]], type = "l", col = "purple",
main = "Yaw (No arthritis set 3)", xlab = "Time (s)", ylab = "Yaw")
plot(Dataset_sub$WallTime, Dataset_sub[[5]], type = "l", col = "orange",
main = "Yaw (No arthritis set 4)", xlab = "Time (s)", ylab = "Yaw")
plot(Dataset_sub$WallTime, Dataset_sub[[6]], type = "l", col = "red",
main = "Yaw (arthritis set 1)", xlab = "Time (s)", ylab = "Yaw")
plot(Dataset_sub$WallTime, Dataset_sub[[7]], type = "l", col = "brown",
main = "Yaw (arthritis set 2)", xlab = "Time (s)", ylab = "Yaw")
anova_result <- aov(yaw_values ~ group_labels)
summary(anova_result)

Pitch sensor
library(tidyverse)
Dataset3_sub <- Dataset3[seq(1, nrow(Dataset3), 5), ]
colnames(Dataset3_sub) <- make.names(colnames(Dataset3_sub))
Dataset3_long <- Dataset3_sub %>%
pivot_longer(cols = starts_with("Pitch"), names_to = "Set", values_to = "PitchValue")
ggplot(Dataset3_long, aes(x = WallTime, y = PitchValue)) +
geom_line(color = "steelblue", linewidth = 0.8) +

facet_wrap(~Set, scales = "free_y") +
labs(title = "Pitch Over Time", x = "Time (WallTime)", y = "Pitch Value") +
theme_minimal(base_size = 12) +
theme(strip.text = element_text(size = 10, face = "bold"),
axis.text.x = element_text(angle = 45, hjust = 1))
anova_result <- aov(PitchValue ~ Set, data = Dataset3_long)
summary(anova_result)

Roll Sensor Plot Code
library(tidyverse)
Another_sub <- Another[seq(1, nrow(Another), 5), ]
colnames(Another_sub) <- make.names(colnames(Another_sub), unique = TRUE)
Another_long <- Another_sub %>%
pivot_longer(cols = starts_with("Roll"), names_to = "Set", values_to = "RollValue")
ggplot(Another_long, aes(x = WallTime, y = RollValue)) +
geom_line(color = "darkblue", linewidth = 0.9) +
facet_wrap(~Set, ncol = 3, scales = "free_y") +
labs(title = "Roll Sensor Readings Over Time", x = "Time (WallTime)", y = "Roll Value")
+
theme_minimal(base_size = 12) +
theme(strip.text = element_text(size = 10, face = "bold"),
axis.text.x = element_text(angle = 45, hjust = 1))
anova_result <- aov(RollValue ~ Set, data = Another_long)
summary(anova_result)

Flex Sensor Plot code

library(tidyverse)
Flex_sub <- Flex[seq(1, nrow(Flex), 5), ]
colnames(Flex_sub) <- make.names(colnames(Flex_sub), unique = TRUE)
Flex_long <- Flex_sub %>%
pivot_longer(cols = starts_with("Flex"), names_to = "Set", values_to = "FlexValue")
ggplot(Flex_long, aes(x = WallTime, y = FlexValue)) +
geom_line(color = "forestgreen", linewidth = 0.9) +
facet_wrap(~Set, ncol = 3, scales = "free_y") +
labs(title = "Flex Sensor Readings Over Time", x = "Time (WallTime)", y = "Flex Value")
+
theme_minimal(base_size = 12) +
theme(strip.text = element_text(size = 10, face = "bold"),
axis.text.x = element_text(angle = 45, hjust = 1))
anova_result <- aov(FlexValue ~ Set, data = Flex_long)
summary(anova_result)

FSR Sensor Plot code
library(tidyverse)
last11_sub <- last11[seq(1, nrow(last11), by = 5), ]
colnames(last11_sub) <- c(
"WallTime",
"FSR no art 1",
"FSR no art 2",
"FSR no art 3",
"FSR no art 4",
"FSR art 1",
"FSR art 2"

)
last11_clean <- last11_sub %>%
drop_na()
last11_long <- last11_clean %>%
pivot_longer(cols = starts_with("FSR"), names_to = "Set", values_to = "FSRValue")
last11_long$Set <- factor(last11_long$Set, levels = c(
"FSR no art 1", "FSR no art 2", "FSR no art 3", "FSR no art 4", "FSR art 1", "FSR art 2"
))
ggplot(last11_long, aes(x = WallTime, y = FSRValue)) +
geom_line(color = "darkorchid", linewidth = 0.9) +
facet_wrap(~Set, ncol = 3, scales = "free_y") +
labs(title = "Cleaned FSR Sensor Data Over Time", x = "Time (WallTime)", y = "FSR
Reading") +
theme_minimal(base_size = 12) +
theme(strip.text = element_text(face = "bold"),
axis.text.x = element_text(angle = 45, hjust = 1))
anova_result <- aov(FSRValue ~ Set, data = last11_long)
summary(anova_result)
