<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from './GridForm.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "./toasts/notifications";
    import Button from './Button.svelte';
    import Text from './Text.svelte';
    import type { RemotesSettings, SpinRemote } from '$lib/types/models';
    import Remote from '~icons/tabler/network';
    import Spinner from './Spinner.svelte';
	import Slider from './Slider.svelte';
	import RemoteComp from './RemoteComp.svelte';
	import Collapsible from './Collapsible.svelte';
	import Checkbox from './Checkbox.svelte';

    export let restPath: string;
    
    let remotes: SpinRemote[] = [];
    let remoteComps: RemoteComp[] = [];
    let remoteSettings: RemotesSettings = {
        isMaster: false,
    }
    let newRemote = {
        hostname: '',
        ip: '',
        rxId: 99
    };

    async function getRemotesSettings() {
        return getJsonRest(restPath, remoteSettings).then((data) => {
            remoteSettings = data;
        });
    }
    async function postRemotesSettings() {
        return postJsonRest(restPath, remoteSettings).then((data) => {
            remoteSettings = data;
        });
    }

    async function getRemotes() {
        return getJsonRest(restPath, { remotes }).then((data) => {
            remotes = data.remotes;
            console.log("Remotes: ", data);
        });
    }

    async function postRemotes() {
        return postJsonRest(restPath, { remotes }).then((data) => {
            notifications.success(`Updated remotes`, 3000);
            console.log("Remotes: ", data);
            return data;
        }).catch((error) => {
            notifications.error(`Failed to update remotes: ${error}`, 3000);
        });
    }

    async function addRemote() {
        if (newRemote.ip || newRemote.rxId) {
            return postJsonRest(restPath, { 
                addRemote: newRemote 
            }).then(() => {
                notifications.success(`Added remote ${newRemote.hostname}`, 3000);
                newRemote = { hostname: '', ip: '', rxId: 99 };
                return getRemotes();
            }).catch((error) => {
                notifications.error(`Failed to add remote: ${error}`, 3000);
            });
        }
    }

    async function removeRemote(index: number) {
        return postJsonRest(restPath, {
            removeRemote: {
                index: index
            }
        }).then(() => {
            notifications.success(`Removed remote ${remotes[index].hostname}`, 3000);
            return getRemotes();
        }).catch((error) => {
            notifications.error(`Failed to remove remote: ${error}`, 3000);
        });
    }

    onMount(() => {
        getRemotes();
        getRemotesSettings();
    });
</script>

<SettingsCard>
    <Remote slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Remotes Manager</span>

    <!-- Remotes list -->
    <Collapsible open={remotes.length > 0}>
        <span slot="title">Registered Remotes ({remotes.length})</span>
        {#if remotes.length === 0}
            <p class="text-gray-500 dark:text-gray-400">No remotes configured</p>
        {:else}
            <div class="grid gap-4">
                {#each remotes as remote, index}
                    <div class="bg-base-200 p-0 rounded-lg flex items-center justify-between gap-2">
                                <!-- Open Link in new tab if IP is set -->
                        {#if remote.ip}
                            <a href={`http://${remote.ip}`} target="_blank" rel="noopener noreferrer">
                            <div>
                                <p class="font-semibold">{remote.hostname}</p>
                                <p class="text-sm text-gray-600 dark:text-gray-400">
                                    IP: {remote.ip || 'Not Connected'} | RX ID: {remote.rxId}
                                </p>
                            </div>
                            </a>
                        {:else}
                            <div>
                                <p class="font-semibold">{remote.hostname}</p>
                                <p class="text-sm text-gray-600 dark:text-gray-400">
                                    IP: {remote.ip || 'Not Connected'} | RX ID: {remote.rxId}
                                </p>
                            </div>
                        {/if}
                        <div class="flex-grow"></div>
                        <Button
                            label="Refresh"
                            onClick={() => {
                                remoteComps[index].getDiag();
                            }}
                        />
                        <Button
                            onClick={() => removeRemote(index)}
                            label="Remove"
                        />
                    </div>
                {/each}
            </div>
        {/if}
    </Collapsible>

    <!-- Add new remote form -->
    <Collapsible>
        <span slot="title">Import Remotes</span>
        <GridForm>
            <!-- <Text
                label="Hostname"
                bind:value={newRemote.hostname}
            /> -->
            <Checkbox
                label="Is Master"
                bind:value={remoteSettings.isMaster}
                onChange={postRemotesSettings}
            />
            <Text
                label="IP Address"
                bind:value={newRemote.ip}
            />
            <Slider
                label="RX ID"
                min={1}
                max={127}
                step={1}
                bind:value={newRemote.rxId}
            />
        </GridForm>

        <div class="flex flex-row flex-wrap justify-between gap-x-2">
            <Button
                onClick={() => postJsonRest('/rest/can', { mapClients: true }).then(() => {
                    setTimeout(() => {
                        getRemotes();
                    }, 2000);
                })}
                label="Scan"
            />
            <Button
                onClick={addRemote}
                label="Add New"
            />
            <div class="flex-grow"></div>
            <Button
                onClick={() => {
                    remotes = [];
                    postJsonRest(restPath, { clearRemotes: true });
                }}
                label="Clear All"
            />
        </div>
    </Collapsible>
</SettingsCard>

{#each remotes as remote, index}
    <RemoteComp
        bind:remote={remote}
        bind:this={remoteComps[index]}
        onChange={postRemotes}
    />
{/each}

<style>
    .grid {
        display: grid;
        gap: 1rem;
    }
</style>