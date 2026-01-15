<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import { notifications } from '$lib/components/toasts/notifications';
	import { getJsonRest, postJsonRest } from '$lib/stores/rest';
	import type { UdpSenderState } from '$lib/types/models';
	import Send from '~icons/tabler/send-2';
	import Broadcast from '~icons/tabler/antenna';
	import Target from '~icons/tabler/target-arrow';

	let sender: UdpSenderState = $state({
		target: '255.255.255.255',
		port: 9600,
		path: '/rest/spin-seq',
		method: 'POST',
		broadcast: true,
		iface: -1,
		lastPayload: '',
		lastResult: '',
		lastError: '',
		lastTarget: '',
		lastPort: 9600,
		lastBroadcast: true,
		lastIface: -1
	});

	let bodyText = $state(`{
		"ping": true
	}`);
	let loading = $state(true);
	let sending = $state(false);
	let formError = $state('');
	let udpMessages: string[] = $state([]);
	let pollTimer: ReturnType<typeof setInterval> | null = null;

	const methods = ['GET', 'POST'];
	const ifaceOptions = [
		{ value: -1, label: 'Auto (prefer Ethernet)' },
		{ value: 2, label: 'Ethernet' },
		{ value: 0, label: 'WiFi STA' }
	];

	onMount(() => {
		loadSender();
		startPolling();
	});

	onDestroy(() => {
		if (pollTimer) {
			clearInterval(pollTimer);
			pollTimer = null;
		}
	});

	function normalizeMethod(value: string): string {
		return (value || 'POST').toUpperCase();
	}

	async function loadSender() {
		loading = true;
		try {
			sender = await getJsonRest('/rest/udp-sender', sender);
			sender.method = normalizeMethod(sender.method);
			udpMessages = await getJsonRest('/rest/spin-seq/udpMessages', []);
		} catch (err) {
			notifications.error('Failed to load UDP sender state', 3000);
		} finally {
			loading = false;
		}
	}

	function startPolling() {
		if (pollTimer) return;
		pollTimer = setInterval(async () => {
			try {
				udpMessages = await getJsonRest('/rest/spin-seq/udpMessages', udpMessages);
			} catch (err) {
				// swallow; UI already has data
			}
		}, 1200);
	}

	function parseBody() {
		formError = '';
		if (bodyText.trim().length === 0) {
			return {};
		}
		try {
			return JSON.parse(bodyText);
		} catch (err) {
			formError = 'Body must be valid JSON.';
			return null;
		}
	}

	async function saveDefaults() {
		sending = true;
		formError = '';
		try {
			const updated = await postJsonRest('/rest/udp-sender', {
				target: sender.target,
				port: sender.port,
				path: sender.path,
				method: normalizeMethod(sender.method),
				broadcast: sender.broadcast,
				iface: Number(sender.iface)
			});
			sender = { ...sender, ...updated };
			notifications.success('UDP defaults saved.', 2500);
		} catch (err) {
			notifications.error('Could not save defaults', 2500);
		} finally {
			sending = false;
		}
	}

	async function sendPacket() {
		const body = parseBody();
		if (body === null) return;

		sending = true;
		try {
			const updated = await postJsonRest('/rest/udp-sender', {
				send: {
					target: sender.target,
					port: sender.port,
					path: sender.path,
					method: normalizeMethod(sender.method),
					broadcast: sender.broadcast,
					iface: Number(sender.iface),
					body
				}
			});
			sender = { ...sender, ...updated };
			notifications.success('UDP packet sent.', 2000);
			udpMessages = await getJsonRest('/rest/spin-seq/udpMessages', udpMessages);
		} catch (err) {
			notifications.error('UDP send failed', 2500);
		} finally {
			sending = false;
		}
	}
</script>

<SettingsCard collapsible={false}>
	{#snippet icon()}
		<Send class="h-6 w-6" />
	{/snippet}
	{#snippet title()}
		<span>UDP Sender</span>
	{/snippet}

	{#if loading}
		<div class="flex w-full items-center justify-center py-8">
			<Spinner />
		</div>
	{:else}
		<div class="w-full flex justify-center">
			<div class="w-full max-w-5xl space-y-4">
				<div class="rounded-box bg-base-200 border border-base-300 p-4">
					<div class="grid grid-cols-1 gap-4 sm:grid-cols-2">
						<label class="form-control w-full">
							<span class="label-text font-semibold flex items-center gap-2"><Target class="h-4 w-4" />Target IP</span>
							<input class="input input-bordered w-full" bind:value={sender.target} placeholder="192.168.1.50 or 255.255.255.255" />
						</label>
						<label class="form-control w-full">
							<span class="label-text font-semibold flex items-center gap-2"><Broadcast class="h-4 w-4" />Broadcast</span>
							<label class="label cursor-pointer justify-start gap-3">
								<input type="checkbox" class="toggle toggle-primary" bind:checked={sender.broadcast} />
								<span class="text-sm opacity-70">Send to 255.255.255.255 when enabled</span>
							</label>
						</label>
						<label class="form-control w-full">
							<span class="label-text font-semibold">Port</span>
							<input type="number" min="1" max="65535" class="input input-bordered w-full" bind:value={sender.port} />
						</label>
						<label class="form-control w-full">
							<span class="label-text font-semibold">Path</span>
							<input class="input input-bordered w-full" bind:value={sender.path} placeholder="/rest/spin-seq/sequencer/" />
						</label>
						<label class="form-control w-full">
							<span class="label-text font-semibold">Method</span>
							<select class="select select-bordered" bind:value={sender.method}>
								{#each methods as method}
									<option value={method}>{method}</option>
								{/each}
							</select>
						</label>
						<label class="form-control w-full">
							<span class="label-text font-semibold">Interface</span>
							<select class="select select-bordered" bind:value={sender.iface}>
								{#each ifaceOptions as opt}
									<option value={opt.value}>{opt.label}</option>
								{/each}
							</select>
						</label>
					</div>
				</div>

				<div class="rounded-box bg-base-200 border border-base-300 p-4">
					<div class="flex items-center justify-between gap-2">
						<div>
							<div class="font-semibold">JSON Body</div>
							<p class="text-sm opacity-70">Payload is wrapped into {'{ method, path, body }'} and delivered via UDP.</p>
						</div>
						<button class="btn btn-ghost btn-xs" type="button" onclick={() => (bodyText = '{\n  "ping": true\n}')}>Reset</button>
					</div>
					<textarea class="textarea textarea-bordered mt-2 h-48 w-full font-mono text-sm" bind:value={bodyText}></textarea>
					{#if formError}
						<p class="text-error text-sm mt-1">{formError}</p>
					{/if}
				</div>

				<div class="flex flex-wrap gap-2">
					<button class="btn btn-primary" type="button" onclick={sendPacket} disabled={sending}>
						<Send class="h-5 w-5" />
						<span>Send UDP</span>
					</button>
					<button class="btn" type="button" onclick={saveDefaults} disabled={sending}>
						<span>Save Defaults</span>
					</button>
				</div>

				<div class="rounded-box bg-base-200 border border-base-300 p-4 space-y-2">
					<div class="font-semibold">Last send</div>
					<div class="text-sm opacity-80">Target: {sender.lastTarget || '—'}</div>
					<div class="text-sm opacity-80">Port: {sender.lastPort}</div>
					<div class="text-sm opacity-80">Broadcast: {sender.lastBroadcast ? 'Yes' : 'No'}</div>
					<div class="text-sm opacity-80">Interface: {sender.lastIface === 2 ? 'Ethernet' : sender.lastIface === 0 ? 'WiFi' : 'Auto'}</div>
					<div class="text-sm opacity-80">Result: {sender.lastResult || 'n/a'}</div>
					{#if sender.lastError}
						<div class="text-sm text-error">Error: {sender.lastError}</div>
					{/if}
					{#if sender.lastPayload}
						<div>
							<div class="text-sm font-semibold mt-2">Last payload</div>
							<pre class="bg-base-300 text-xs p-2 rounded-box overflow-x-auto">{sender.lastPayload}</pre>
						</div>
					{/if}
				</div>

				<div class="rounded-box bg-base-200 border border-base-300 p-4 space-y-2">
					<div class="flex items-center justify-between">
						<div class="font-semibold">UDP Console</div>
						<button class="btn btn-ghost btn-xs" type="button" onclick={async () => udpMessages = await getJsonRest('/rest/spin-seq/udpMessages', udpMessages)}>
							Refresh
						</button>
					</div>
					<div class="text-sm opacity-70">Live view of payloads received on /rest/spin-seq via UDP.</div>
					<div class="bg-base-300 rounded-box p-2 h-64 overflow-y-auto space-y-1 text-xs font-mono">
						{#if udpMessages.length === 0}
							<div class="opacity-60">No UDP messages received yet.</div>
						{:else}
							{#each [...udpMessages].reverse() as msg, idx}
								<div class="whitespace-pre-wrap break-words">{msg}</div>
							{/each}
						{/if}
					</div>
				</div>
			</div>
		</div>
	{/if}
</SettingsCard>
