<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { getJsonRest } from '$lib/stores/rest';
	import Spinner from '$lib/components/Spinner.svelte';
	import type { SpinDiagnostics } from '$lib/types/models';
	import Info from '~icons/tabler/info-circle';

	onMount(() => {
	});

	onDestroy(() => {
	});

	export let restPath = 'rest/spin-seq/diag';

	let diag: SpinDiagnostics = {} as SpinDiagnostics;

	async function getDiag() {
		return getJsonRest(restPath, diag).then((data) => {
			diag = data;
			return diag;
		});
	}

</script>

<SettingsCard>
	<Info slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Status</span>
	{#await getDiag()}
	<Spinner></Spinner>
	{:then diag}
		<div class="grid grid-cols-1 md:grid-cols-2 gap-4">
			<!-- MCU Section -->
			<div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2 dark:text-gray-200">MCU</h3>
				<div class="space-y-2">
					<p class="text-sm dark:text-gray-300">Version: <span class="font-mono">{diag.mcu.version}</span></p>
					<p class="text-sm dark:text-gray-300">Free Heap: <span class="font-mono">{diag.mcu.freeHeap/1000} kB</span></p>
					<p class="text-sm dark:text-gray-300">Free Sketch Space: <span class="font-mono">{diag.mcu.freeSketchSpace/1000} kB</span></p>
                    <p class="text-sm dark:text-gray-300">CPU Frequency: <span class="font-mono">{diag.mcu.cpuFreq} Hz</span></p>
				</div>
			</div>

            <!-- Sequencer Section -->
            <div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
                <h3 class="text-lg font-semibold mb-2 dark:text-gray-200">Sequencer</h3>
                <div class="space-y-2">
                    <p class="text-sm dark:text-gray-300">Running: 
                        <span class={diag.sequencer.isRunning ? "text-green-600 dark:text-green-500" : "text-red-600 dark:text-red-500"}>
                            {diag.sequencer.isRunning ? "Yes" : "No"}
                        </span>
                    </p>
                    <p class="text-sm dark:text-gray-300">Number of Commands: <span class="font-mono">{diag.sequencer.numCommands}</span></p>
                    <p class="text-sm dark:text-gray-300">Selected Command: <span class="font-mono">{diag.sequencer.selectedCommand}</span></p>
                    <p class="text-sm dark:text-gray-300">Next Command: <span class="font-mono">{diag.sequencer.nextCommand}</span></p>
                </div>
            </div>

			<!-- Stepper Section -->
			<div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2 dark:text-gray-200">Stepper</h3>
				<div class="space-y-2">
					<p class="text-sm dark:text-gray-300">Status: <span class="font-mono">{diag.stepper.status}</span></p>
					<p class="text-sm dark:text-gray-300">Version: <span class="font-mono">{diag.stepper.version}</span></p>
					<p class="text-sm dark:text-gray-300">
						Enabled: 
						<span class={diag.stepper.isEnabled ? "text-green-600 dark:text-green-500" : "text-red-600 dark:text-red-500"}>
							{diag.stepper.isEnabled ? "Yes" : "No"}
						</span>
					</p>
				</div>
			</div>

			<!-- Encoder Section -->
			<div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2 dark:text-gray-200">Encoder</h3>
				<div class="space-y-2">
					<p class="text-sm dark:text-gray-300">Angle: <span class="font-mono">{diag.encoder.angle.toFixed(2)}°</span></p>
					<p class="text-sm dark:text-gray-300">Error: <span class={diag.encoder.error ? "text-red-600 dark:text-red-500" : "text-green-600 dark:text-green-500"}>{diag.encoder.error ? 'Yes' : 'No'}</span></p>
					<p class="text-sm dark:text-gray-300">Invert: <span class={diag.encoder.invert ? "text-green-600 dark:text-green-500" : "text-red-600 dark:text-red-500"}>{diag.encoder.invert ? 'Yes' : 'No'}</span></p>
					<p class="text-sm dark:text-gray-300">New Data: <span class={diag.encoder.newData ? "text-green-600 dark:text-green-500" : "text-red-600 dark:text-red-500"}>{diag.encoder.newData ? 'Yes' : 'No'}</span></p>
				</div>
			</div>

			<!-- WiFi Section -->
			<div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2 dark:text-gray-200">WiFi</h3>
				<div class="space-y-2">
					<p class="text-sm dark:text-gray-300">SSID: <span class="font-mono">{diag.wifi.ssid || 'Not Connected'}</span></p>
                    <p class="text-sm dark:text-gray-300">Hostname: <span class="font-mono">{diag.wifi.hostname}</span></p>
					<p class="text-sm dark:text-gray-300">IP: <span class="font-mono">{diag.wifi.ip || 'Not Available'}</span></p>
					<p class="text-sm dark:text-gray-300">AP Enabled: <span class="font-mono">{diag.wifi.apEnabled ? 'Yes' : 'No'}</span></p>
					<p class="text-sm dark:text-gray-300">RSSI: <span class="font-mono">{diag.wifi.rssi} dBm</span></p>
				</div>
			</div>

			<!-- CAN Section -->
			<div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2 dark:text-gray-200">CAN</h3>
				<div class="space-y-2">
					<p class="text-sm dark:text-gray-300">Enabled: <span class="font-mono">{diag.can.enabled ? 'Yes' : 'No'}</span></p>
					<p class="text-sm dark:text-gray-300">RX ID: <span class="font-mono">{diag.can.rxId}</span></p>
					<p class="text-sm dark:text-gray-300">Speed: <span class="font-mono">{diag.can.speed} kBps</span></p>
					<p class="text-sm dark:text-gray-300">Messagepack: <span class="font-mono">{diag.can.messagePack ? 'Yes' : 'No'}</span></p>
				</div>
			</div>

			<!-- HTTP Section -->
			<div class="bg-gray-50 dark:bg-gray-800 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2 dark:text-gray-200">HTTP</h3>
				<div class="space-y-2">
					<p class="text-sm dark:text-gray-300">Max Clients: <span class="font-mono">{diag.http.maxClients}</span></p>
					<p class="text-sm dark:text-gray-300">Current Clients: <span class="font-mono">{diag.http.numClients}</span></p>
					<p class="text-sm dark:text-gray-300">Max URI Handlers: <span class="font-mono">{diag.http.maxUriHandlers}</span></p>
                    <p class="text-sm dark:text-gray-300">LRU Purge Enable: <span class="font-mono">{diag.http.lruPurgeEnable ? 'Yes' : 'No'}</span></p>
				</div>
			</div>
		</div>
	{/await}
</SettingsCard>