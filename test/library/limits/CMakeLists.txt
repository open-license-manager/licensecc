# DateVerifier tests
add_executable(
	test_date_verifier
	date_verifier_test.cpp
)

target_link_libraries(
	test_date_verifier
	licensecc
	${Boost_LIBRARIES}
)

# PcSignatureVerifier tests
add_executable(
	test_pc_signature_verifier
	pc_signature_verifier_test.cpp
)

target_link_libraries(
	test_pc_signature_verifier
	licensecc
	${Boost_LIBRARIES}
)

# LicenseVerifier tests
add_executable(
	test_license_verifier
	license_verifier_test.cpp
)

target_link_libraries(
	test_license_verifier
	licensecc
	${Boost_LIBRARIES}
)

ADD_TEST(NAME test_date_verifier COMMAND test_date_verifier)
ADD_TEST(NAME test_pc_signature_verifier COMMAND test_pc_signature_verifier)
ADD_TEST(NAME test_license_verifier COMMAND test_license_verifier)