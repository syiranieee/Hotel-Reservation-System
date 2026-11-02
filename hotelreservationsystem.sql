-- phpMyAdmin SQL Dump
-- version 5.2.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 20, 2026 at 04:35 PM
-- Server version: 10.4.27-MariaDB
-- PHP Version: 8.1.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `hotelreservationsystem`
--

-- --------------------------------------------------------

--
-- Table structure for table `booking`
--

CREATE TABLE `booking` (
  `Booking_ID` int(11) NOT NULL,
  `User_ID` int(11) DEFAULT NULL,
  `Room_ID` int(11) DEFAULT NULL,
  `CheckIn_Date` date DEFAULT NULL,
  `CheckOut_Date` date DEFAULT NULL,
  `TotalPrice` decimal(10,2) DEFAULT NULL,
  `BookingStatus` varchar(20) DEFAULT 'Confirmed'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `booking`
--

INSERT INTO `booking` (`Booking_ID`, `User_ID`, `Room_ID`, `CheckIn_Date`, `CheckOut_Date`, `TotalPrice`, `BookingStatus`) VALUES
(2, 6, 5, '2025-07-15', '2025-07-16', '1200.00', 'Completed'),
(3, 7, 2, '2025-08-05', '2025-08-07', '500.00', 'Completed'),
(4, 8, 3, '2025-08-20', '2025-08-23', '1350.00', 'Completed'),
(5, 9, 4, '2025-09-12', '2025-09-14', '700.00', 'Completed'),
(7, 11, 5, '2025-10-02', '2025-10-03', '1200.00', 'Completed'),
(8, 12, 2, '2025-10-15', '2025-10-18', '750.00', 'Completed'),
(9, 13, 3, '2025-11-10', '2025-11-12', '900.00', 'Completed'),
(10, 14, 4, '2025-11-20', '2025-11-22', '700.00', 'Completed'),
(11, 15, 5, '2025-12-24', '2025-12-26', '2400.00', 'Completed'),
(14, 18, 3, '2026-01-05', '2026-01-07', '900.00', 'Completed'),
(16, 20, 2, '2026-02-05', '2026-02-07', '500.00', 'Confirmed'),
(17, 5, 2, '2026-02-05', '2026-02-07', '560.00', 'Confirmed'),
(18, 25, 3, '2026-01-18', '2026-01-20', '900.00', 'Cancelled'),
(19, 5, 2, '2026-01-10', '2026-01-11', '350.00', 'Checked-In'),
(21, 6, 5, '2025-12-05', '2025-12-06', '1200.00', 'Completed'),
(22, 7, 2, '2025-12-10', '2025-12-12', '560.00', 'Completed'),
(23, 8, 3, '2025-12-15', '2025-12-18', '1350.00', 'Completed'),
(24, 15, 5, '2025-12-20', '2025-12-22', '2400.00', 'Completed'),
(26, 5, 2, '2026-01-10', '2026-01-12', '560.00', 'Checked-In'),
(27, 9, 4, '2026-01-15', '2026-01-18', '1050.00', 'Completed'),
(29, 11, 8, '2026-02-01', '2026-02-03', '1200.00', 'Cancelled'),
(30, 20, 3, '2026-02-10', '2026-02-14', '1800.00', 'Confirmed'),
(31, 25, 2, '2026-02-15', '2026-02-17', '560.00', 'Confirmed'),
(32, 18, 3, '2026-01-05', '2026-01-07', '900.00', 'Cancelled'),
(34, 16, 4, '2026-02-14', '2026-02-15', '350.00', 'Confirmed'),
(35, 5, 5, '2026-02-19', '2026-02-20', '1200.00', 'Confirmed'),
(36, 25, 4, '2026-01-13', '2026-01-14', '350.00', 'Cancelled'),
(37, 25, 8, '2026-01-14', '2026-01-15', '600.00', 'Confirmed'),
(38, 15, 3, '2026-01-12', '2026-01-13', '450.00', 'Cancelled');

-- --------------------------------------------------------

--
-- Table structure for table `customer`
--

CREATE TABLE `customer` (
  `Customer_ID` int(11) NOT NULL,
  `User_Id` int(11) DEFAULT NULL,
  `Name` varchar(100) DEFAULT NULL,
  `IC_Number` varchar(20) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `customer`
--

INSERT INTO `customer` (`Customer_ID`, `User_Id`, `Name`, `IC_Number`) VALUES
(1, 5, 'Shoffiyah', '010101-01-1111'),
(2, 6, 'Aiman Hakimi', '010101-01-2222'),
(3, 7, 'Mohd Aimi', '010101-01-3333'),
(4, 8, 'Hanif', '010101-01-4444'),
(5, 9, 'Irfan', '010101-01-5555'),
(6, 10, 'Maisarah', '010101-01-6666'),
(7, 11, 'Khalis Fikri', '010101-01-7777'),
(8, 12, 'Mohd Rizuan', '010101-01-8888'),
(9, 13, 'Amzar', '010101-01-9999'),
(10, 14, 'Teh Yuan', '010101-01-1010'),
(11, 15, 'Neoh Xinyi', '010101-01-1110'),
(12, 16, 'Nadhirah Najwa', '010101-01-1212'),
(14, 18, 'Ksherja', '010101-01-1414'),
(15, 19, 'Ammar Danish', '010101-01-1515'),
(16, 20, 'Qistina Raisah', '010101-01-1616'),
(17, 21, 'Daiyan Trisha', '010101-01-1717'),
(18, 22, 'Syafiq Kyle', '010101-01-1818'),
(19, 23, 'Nadhir Nasar', '010101-01-1919'),
(20, 24, 'Taylor Swift', '010101-01-2020'),
(21, 25, 'Hagea Sophea', '931120-07-2222'),
(22, 28, 'Ainul Nazirah', '940805080424');

-- --------------------------------------------------------

--
-- Table structure for table `reward`
--

CREATE TABLE `reward` (
  `Reward_ID` int(11) NOT NULL,
  `Customer_ID` int(11) DEFAULT NULL,
  `Points` int(11) DEFAULT 0,
  `Level` varchar(20) DEFAULT 'Bronze'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `reward`
--

INSERT INTO `reward` (`Reward_ID`, `Customer_ID`, `Points`, `Level`) VALUES
(1, 1, 206, 'Silver'),
(2, 2, 120, 'Silver'),
(3, 3, 150, 'Silver'),
(4, 4, 150, 'Silver'),
(5, 5, 1050, 'Gold'),
(6, 6, 150, 'Silver'),
(7, 7, 150, 'Silver'),
(8, 8, 150, 'Silver'),
(9, 9, 150, 'Silver'),
(10, 10, 150, 'Silver'),
(11, 11, 110, 'Silver'),
(12, 12, 185, 'Silver'),
(14, 14, 150, 'Silver'),
(15, 15, 150, 'Silver'),
(16, 16, 180, 'Silver'),
(17, 17, 165, 'Silver'),
(18, 18, 150, 'Silver'),
(19, 19, 150, 'Silver'),
(20, 20, 150, 'Silver'),
(32, 21, 116, 'Silver'),
(33, 22, 0, 'Bronze');

-- --------------------------------------------------------

--
-- Table structure for table `role`
--

CREATE TABLE `role` (
  `RoleID` int(11) NOT NULL,
  `Position` varchar(50) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `role`
--

INSERT INTO `role` (`RoleID`, `Position`) VALUES
(1, 'Manager'),
(2, 'Receptionist'),
(3, 'Customer'),
(4, 'Staff');

-- --------------------------------------------------------

--
-- Table structure for table `room`
--

CREATE TABLE `room` (
  `Room_ID` int(11) NOT NULL,
  `RoomType` varchar(50) DEFAULT NULL,
  `RoomPrice` decimal(10,2) DEFAULT NULL,
  `TotalQuantity` int(11) DEFAULT NULL,
  `RoomStatus` varchar(20) DEFAULT 'Available',
  `RoomDescription` text DEFAULT NULL,
  `Facilities` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `room`
--

INSERT INTO `room` (`Room_ID`, `RoomType`, `RoomPrice`, `TotalQuantity`, `RoomStatus`, `RoomDescription`, `Facilities`) VALUES
(2, 'Standard Queen', '280.00', 14, 'Occupied', 'Two twin beds with a city view.', 'WiFi, TV, AC, Mini Bar'),
(3, 'Executive Suite', '450.00', 10, 'Available', 'Luxury suite with a private living area.', 'WiFi, Smart TV, AC, Bathtub, Breakfast included'),
(4, 'Family Room', '350.00', 11, 'Available', 'Large room with one king and one single bed.', 'WiFi, TV, AC, Extra Pillows'),
(5, 'Presidential Suite', '1200.00', 3, 'Available', 'Our most luxurious room on the top floor.', 'Private Balcony, Jacuzzi, Butler Service, Full Bar'),
(8, 'Superior Deluxe', '600.00', 5, 'Maintenance', 'Expansive premium room with elegant design.', 'King Bed, Smart TV, Wi-Fi, Bathtub, Mini-bar, Executive Desk'),
(9, 'Honeymoon Room', '500.00', 1, 'Available', 'Honeymoon', 'Double queen bed');

-- --------------------------------------------------------

--
-- Table structure for table `staff`
--

CREATE TABLE `staff` (
  `Staff_ID` int(11) NOT NULL,
  `User_ID` int(11) DEFAULT NULL,
  `StaffName` varchar(100) DEFAULT NULL,
  `StaffPosition` varchar(50) DEFAULT NULL,
  `StaffShiftTime` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `staff`
--

INSERT INTO `staff` (`Staff_ID`, `User_ID`, `StaffName`, `StaffPosition`, `StaffShiftTime`) VALUES
(1, 1, 'Ayinnajwa', 'Manager', 'Morning'),
(2, 2, 'Nurhani', 'Receptionist', 'Evening'),
(3, 3, 'Wahida', 'Receptionist', 'Night'),
(4, 4, 'Nurazlyn', 'Receptionist', 'Morning'),
(5, 26, 'Hanis Zalikha ', 'Receptionist', 'Night'),
(6, 27, 'Liya Maisarah', 'Staff', 'morning'),
(7, 29, 'Nur Alya', 'Staff', 'Morning');

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

CREATE TABLE `user` (
  `User_ID` int(11) NOT NULL,
  `Username` varchar(50) NOT NULL,
  `Password` varchar(255) NOT NULL,
  `RoleID` int(11) DEFAULT NULL,
  `User_Email` varchar(100) DEFAULT NULL,
  `User_PhoneNumber` varchar(20) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `user`
--

INSERT INTO `user` (`User_ID`, `Username`, `Password`, `RoleID`, `User_Email`, `User_PhoneNumber`) VALUES
(1, 'ayinnajwa', 'ayinnajwa123', 1, 'ayin@hotel.com', '011222333'),
(2, 'nurhani', 'nurhani123', 2, 'hani@hotel.com', '011222334'),
(3, 'wahida', 'wahida123', 2, 'wahida@hotel.com', '011222335'),
(4, 'nurazlyn', 'nurazlyn123', 2, 'azlyn@hotel.com', '011222336'),
(5, 'shoffiyah', 'shoffiyah123', 3, 'shoffiyah@gmail.com', '0123456701'),
(6, 'aiman', 'aiman123', 3, 'aiman@gmail.com', '0123456702'),
(7, 'aimi', 'aimi123', 3, 'aimi@gmail.com', '0123456703'),
(8, 'hanif', 'hanif123', 3, 'hanif@gmail.com', '0123456704'),
(9, 'irfan', 'irfan123', 3, 'irfan@gmail.com', '0123456705'),
(10, 'maisarah', 'maisarah123', 3, 'maisarah@gmail.com', '0123456706'),
(11, 'khalis', 'khalis123', 3, 'khalis@gmail.com', '0123456707'),
(12, 'rizuan', 'rizuan123', 3, 'rizuan@gmail.com', '0123456708'),
(13, 'amzar', 'amzar123', 3, 'amzar@gmail.com', '0123456709'),
(14, 'yuan', 'yuan123', 3, 'yuan@gmail.com', '0123456710'),
(15, 'xinyi', 'xinyi123', 3, 'xinyi@gmail.com', '0123456711'),
(16, 'nadhirah', 'nadhirah123', 3, 'nadhirah@gmail.com', '0123456712'),
(18, 'ksherja', 'ksherja123', 3, 'ksherja@gmail.com', '0123456714'),
(19, 'ammar', 'ammar123', 3, 'ammar@gmail.com', '0123456715'),
(20, 'qistina', 'qistina123', 3, 'qistina@gmail.com', '0123456716'),
(21, 'daiyan', 'daiyan123', 3, 'daiyan@gmail.com', '0123456717'),
(22, 'syafiq', 'syafiq123', 3, 'syafiq@gmail.com', '0123456718'),
(23, 'nadhir', 'nadhir123', 3, 'nadhir@gmail.com', '0123456719'),
(24, 'taylorswift', 'taylor123', 3, 'taylor@gmail.com', '01212120047'),
(25, 'hageasophea', 'hageasophea123', 3, 'hageasophea@email.com', '0134331122'),
(26, 'haniszalikha', 'haniszalikha123', 2, 'haniszalikha@email.com', '01344225558'),
(27, 'liyamaisarah', 'liya123', 4, 'liya@gmail.com', '01343330225'),
(28, 'aiulnazirah', 'ainul123', 3, 'ainul@gmail.com', '0134335565'),
(29, 'nuralya', 'alya123', 4, 'alya@gmail.com', '0142525367');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `booking`
--
ALTER TABLE `booking`
  ADD PRIMARY KEY (`Booking_ID`),
  ADD KEY `User_ID` (`User_ID`),
  ADD KEY `Room_ID` (`Room_ID`);

--
-- Indexes for table `customer`
--
ALTER TABLE `customer`
  ADD PRIMARY KEY (`Customer_ID`),
  ADD KEY `User_Id` (`User_Id`);

--
-- Indexes for table `reward`
--
ALTER TABLE `reward`
  ADD PRIMARY KEY (`Reward_ID`),
  ADD KEY `Customer_ID` (`Customer_ID`);

--
-- Indexes for table `role`
--
ALTER TABLE `role`
  ADD PRIMARY KEY (`RoleID`);

--
-- Indexes for table `room`
--
ALTER TABLE `room`
  ADD PRIMARY KEY (`Room_ID`);

--
-- Indexes for table `staff`
--
ALTER TABLE `staff`
  ADD PRIMARY KEY (`Staff_ID`),
  ADD KEY `User_ID` (`User_ID`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`User_ID`),
  ADD UNIQUE KEY `Username` (`Username`),
  ADD KEY `RoleID` (`RoleID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `booking`
--
ALTER TABLE `booking`
  MODIFY `Booking_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=40;

--
-- AUTO_INCREMENT for table `customer`
--
ALTER TABLE `customer`
  MODIFY `Customer_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=23;

--
-- AUTO_INCREMENT for table `reward`
--
ALTER TABLE `reward`
  MODIFY `Reward_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=34;

--
-- AUTO_INCREMENT for table `role`
--
ALTER TABLE `role`
  MODIFY `RoleID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `room`
--
ALTER TABLE `room`
  MODIFY `Room_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=10;

--
-- AUTO_INCREMENT for table `staff`
--
ALTER TABLE `staff`
  MODIFY `Staff_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- AUTO_INCREMENT for table `user`
--
ALTER TABLE `user`
  MODIFY `User_ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=30;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `booking`
--
ALTER TABLE `booking`
  ADD CONSTRAINT `booking_ibfk_1` FOREIGN KEY (`User_ID`) REFERENCES `user` (`User_ID`) ON DELETE CASCADE,
  ADD CONSTRAINT `booking_ibfk_2` FOREIGN KEY (`Room_ID`) REFERENCES `room` (`Room_ID`) ON DELETE CASCADE;

--
-- Constraints for table `customer`
--
ALTER TABLE `customer`
  ADD CONSTRAINT `customer_ibfk_1` FOREIGN KEY (`User_Id`) REFERENCES `user` (`User_ID`) ON DELETE CASCADE;

--
-- Constraints for table `reward`
--
ALTER TABLE `reward`
  ADD CONSTRAINT `reward_ibfk_1` FOREIGN KEY (`Customer_ID`) REFERENCES `customer` (`Customer_ID`) ON DELETE CASCADE;

--
-- Constraints for table `staff`
--
ALTER TABLE `staff`
  ADD CONSTRAINT `staff_ibfk_1` FOREIGN KEY (`User_ID`) REFERENCES `user` (`User_ID`) ON DELETE CASCADE;

--
-- Constraints for table `user`
--
ALTER TABLE `user`
  ADD CONSTRAINT `user_ibfk_1` FOREIGN KEY (`RoleID`) REFERENCES `role` (`RoleID`) ON DELETE SET NULL;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
