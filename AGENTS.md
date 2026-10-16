# DIY cuckoo clock project rules

Read CODEX_TASK_STATE.md and hardware/BOM.md before continuing. Preserve the user's purchased parts and the already installed quartz clock.

- Separate software simulation, rendered diagrams, and real hardware verification in every claim.
- Never invent pin order, connector direction, mounting dimensions, or a physically validated assembly from an order thumbnail.
- The actual single horn is the short one-sided white horn. Do not assume 2.7mm ties fit its small holes.
- Keep two doors simultaneous; bird out after doors open, bird home before closing; quiet hours 21:00–08:00; 12-hour count.
- Keep quartz hands independent from RTC. No front-hand synchronization requirement.
- Teach one observable action at a time with the real part, matching view direction, insertion/motion close-up, and a completion check. Static PDF narration is not sufficient.
- Anatomy may assist geometry and motion. Its invented-object/no-label aesthetic must yield to accurate parts, Chinese labels, wire colours and child comprehension. Its geometric checks do not validate electrical safety or hardware fit.
- Never commit private/, order screenshots, home paths, account data, or ambiguous-license imagery.
- Run python3 tools/test_firmware.py after firmware changes. Record hardware limits and actual validation separately.
